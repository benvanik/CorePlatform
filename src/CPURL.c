/*
 *  CPURL.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/18/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CPURL.h>

CP_DEFINE_TYPE(CPURL, NULL, CPURLDealloc);

// NOTE: imported from CPString.c - this is nasty, but efficient
sal_out size_t _CPStringGetTotalByteLength(const size_t length);
sal_checkReturn sal_out_opt CPStringRef _CPStringCreateEmpty(const size_t length);

BOOL _CPURLIsRooted(sal_in_z const CPChar* url)
{
    // Looking for [a-zA-Z0-9]+:
    const CPChar* p = url;
    while (*p) {
        const CPChar c = *p;
        if (c == ':') {
            return p != url;
        }
        if ((('a' <= c) && (c <= 'z')) ||
            (('A' <= c) && (c <= 'Z')) ||
            (('0' <= c) && (c <= '9'))) {
            //
            p++;
            continue;
        } else {
            return FALSE;
        }
    }
    return FALSE;
}

size_t _CPURLMeasureRootLength(sal_inout CPURLRef url)
{
    // Walk up until at the top
    while (url->baseURL) {
        url = url->baseURL;
    }

    // Verify a scheme: is present
    if (!_CPURLIsRooted(url->value)) {
        return -1;
    }

    // Skip over scheme:[//]
    const CPChar* p = CPStrChr(url->value, ':');
    if (!p) {
        return -1;
    }
    p++; // skip :
    size_t slashCount = 0;
    while (*p) {
        if (*p != '/') {
            break;
        }
        slashCount++;
        p++;
    }
    if (!*p) {
        // End of string after scheme:[//] - return whole length
        // Only accept 3 slashes if nothing else
        // scheme:///
        if (slashCount == 3) {
            // Don't include the 3rd trailing slash
            return url->length - 1;
        } else {
            return -1;
        }
    }
    // Back up on the slashes a bit
    while (slashCount > 2) {
        slashCount--;
        p--;
    }

    // Find the next / after scheme://
    const CPChar* slash = CPStrChr(p, '/');
    size_t length;
    if (slash) {
        // Measure from start to / (exclusive)
        length = slash - url->value;
    } else {
        // No trailing /, so entire value is it
        length = url->length;
    }

    // Basic validation - ensure we have a host (or are at least :///) and port is ok
    // TODO: better validation of root?
    if ((url->value[length - 1] == ':') ||
        (url->value[length - 1] == '@')) {
        return -1;
    }

    return length;
}

size_t _CPURLCopyRoot(sal_inout CPURLRef url, sal_out_bcount(bufferSize) CPChar* buffer, const size_t bufferSize)
{
    // Walk up until at the top
    while (url->baseURL) {
        url = url->baseURL;
    }

    // Hacky, but grab root length
    const size_t rootLength = _CPURLMeasureRootLength(url);
    if (rootLength == -1) {
        return -1;
    }

    // Copy in url up to rootLength
    size_t rootSize;
    if (!CPMultSizeT(rootLength, sizeof(CPChar), &rootSize)) {
        return -1;
    }
    if (!CPCopyMemory(buffer, bufferSize, url->value, rootSize)) {
        return -1;
    }

    // Add trailing / if needed
    if (buffer[rootLength - 1] != '/') {
        buffer[rootLength] = '/';

        // Add NUL
        buffer[rootLength + 1] = 0;
    } else {
        // Add NUL
        buffer[rootLength] = 0;
    }

    return rootLength;
}

sal_checkReturn BOOL _CPURLConstructAbsoluteString(sal_inout CPURLRef url, sal_out_bcount(bufferSize) CPChar* buffer, const size_t bufferSize)
{
    if (!url->baseURL && _CPURLIsRooted(url->value)) {
        // Likely a rooted URL, e.g. http://host/path
        if (_CPURLMeasureRootLength(url) == -1) {
            return FALSE;
        }
        return CPStrCpy(buffer, bufferSize / sizeof(CPChar), url->value);
    } else if (url->baseURL) {
        // Relative URL
        if (url->value[0] == '/') {
            // Absolute path - quickly get the root and add to self
            const size_t rootLength = _CPURLCopyRoot(url, buffer, bufferSize);
            if (rootLength == -1) {
                return FALSE;
            }
            size_t remainingBuffer = bufferSize / sizeof(CPChar) - rootLength;
            // Append self
            return CPStrCpy(buffer + rootLength, remainingBuffer, url->value);
        } else {
            // Relative path - need to construct entire parent and combine the URLs to check
            // TODO:
            return FALSE;
        }
    } else {
        // Not root and no base, so invalid?
        return FALSE;
    }
}

size_t _CPURLMeasureAbsoluteLength(sal_inout CPURLRef url)
{
    // TODO: increase buffer size to get larger max URLs?
    CPChar buffer[2048];
    if (!_CPURLConstructAbsoluteString(url, buffer, sizeof(buffer))) {
        return -1;
    }
    return CPStrLen(buffer);
}

// NOTE: this is a utility method and should not be exposed!
sal_checkReturn sal_out_opt CPURLRef _CPURLCreateEmpty(sal_inout_opt CPURLRef baseURL, const size_t length)
{
    CPURLRef url = NULL;

    // Nasty, disgustingly safe math that computes:
    // totalSize = sizeof(CPURL) + ((length + 1) * sizeof(CPChar))
    const size_t totalValueBytes = _CPStringGetTotalByteLength(length);
    size_t totalSize;
    CPEXPECTTRUE(CPAddSizeT(sizeof(CPURL), totalValueBytes, &totalSize));

    // Allocate enough room for the object plus the value
    url = (CPURLRef)CPObjectAlloc(&CPURLType, totalSize);
    CPEXPECTNOTNULL(url);

    url->baseURL    = CPURLRetain(baseURL);

    url->length     = length;

    // Add the NUL terminator
    url->value[0]   = 0;

    return url;

CPCLEANUP:
    CPRelease(url);
    return NULL;
}

CP_API sal_checkReturn sal_out_opt CPURLRef CPURLCreate(sal_inout_opt CPURLRef baseURL, sal_in_z const CPChar* source)
{
    CPURLRef url = NULL;

    const size_t sourceLength = CPStrLen(source);
    if (!sourceLength) {
        // No additional path so just retain and return the given base
        CPURLRetain(baseURL);
        return baseURL;
    }

    // If source is a rooted URL (scheme://) then ignore baseURL
    // This optimization makes computing absolute paths easier as we can quickly identify roots
    if (baseURL && _CPURLIsRooted(source)) {
        // May not be correct, but likely is - validation will ensure it below
        baseURL = NULL;
    }

    // Create empty URL of appropriate length
    url = _CPURLCreateEmpty(baseURL, sourceLength);
    CPEXPECTNOTNULL(url);

    // Compute how many bytes we should copy
    size_t sourceValueBytes;
    CPEXPECTTRUE(CPMultSizeT(sourceLength, sizeof(CPChar), &sourceValueBytes));

    // Copy in the string (this does not include a NUL terminator)
    CPEXPECTTRUE(CPCopyMemory(url->value, sourceValueBytes, source, sourceValueBytes));

    // Add the NUL terminator
    url->value[sourceLength] = 0;

    // A little late to do the check, but verify the URL is (somewhat) valid and cache absolute length
    url->absoluteLength = _CPURLMeasureAbsoluteLength(url);
    CPEXPECT(url->absoluteLength != 0);
    CPEXPECT(url->absoluteLength != -1);

    return url;

CPCLEANUP:
    CPRelease(url);
    return NULL;
}

CP_API sal_checkReturn sal_out_opt CPURLRef CPURLCreateWithString(sal_inout_opt CPURLRef baseURL, sal_inout CPStringRef source)
{
    return CPURLCreate(baseURL, source->value);
}

CP_API sal_checkReturn sal_out_opt CPURLRef CPURLCreateAbsoluteCopy(sal_inout CPURLRef source)
{
    const size_t length = source->absoluteLength;
    if (length == -1) {
        return NULL;
    }

    CPURLRef url = _CPURLCreateEmpty(NULL, length);
    CPEXPECTNOTNULL(url);

    CPEXPECTTRUE(CPURLGetAbsoluteString(source, url->value, length));

    // NOTE: no validation is performed because we assume that the source URL could not have been created incorrectly

    return url;

CPCLEANUP:
    CPRelease(url);
    return NULL;
}

sal_callback void CPURLDealloc(sal_inout CPURLRef url)
{
    // NOTE: no need to free value as it is part of the URL structure

    CPRelease(url->baseURL);

#if defined(CP_DEBUG)
    // Fill the value with garbage to make it easier to spot use-after-free errors
    for (size_t n = 0; n < url->length; n++) {
        url->value[n] = 0x253;
    }
#endif
}

CP_API sal_out_opt CPStringRef CPURLCopyAbsoluteString(sal_inout CPURLRef url)
{
    const size_t length = url->absoluteLength;
    if (length == -1) {
        return NULL;
    }

    CPStringRef string = _CPStringCreateEmpty(length);
    CPEXPECTNOTNULL(string);

    const size_t totalBytes = _CPStringGetTotalByteLength(length);
    CPEXPECTTRUE(CPURLGetAbsoluteString(url, string->value, totalBytes));

    return string;

CPCLEANUP:
    CPRelease(string);
    return NULL;
}

CP_API sal_checkReturn BOOL CPURLGetAbsoluteString(sal_inout CPURLRef url, sal_out_bcount(bufferSize) CPChar* buffer, const size_t bufferSize)
{
    const size_t totalBytes = _CPStringGetTotalByteLength(url->absoluteLength);
    if (totalBytes > bufferSize) {
        return FALSE;
    }

    return _CPURLConstructAbsoluteString(url, buffer, bufferSize);
}

// URL escape/unescape comes from:
// http://www.icosaedro.it/apache/urlencode.c
// http://www.icosaedro.it/apache/urldecode.c
// Slight modifications made to support paths
// http://www.faqs.org/rfcs/rfc1738.html

CP_API sal_checkReturn size_t CPURLEscape(sal_in_z const CPChar* source, sal_out_bcount(bufferSize) CPChar* buffer, const size_t bufferSize, const uint32 escapeOptions)
{
    const CPChar* hex = CPTEXT("0123456789abcdef");

    // Require room for at least one character (NUL)
    if (bufferSize < sizeof(CPChar)) {
        return -1;
    }

    // For each character in the source string, copy or escape to the buffer
    size_t index = 0;
    const CPChar* sp = source;
    while (*sp) {
        const CPChar c = *sp;

        // Error out on non-ASCII characters
        if (c > 0x255) {
            return -1;
        }

        // Determine if character needs escaping
        BOOL needsEscaping = TRUE;
        if ((('a' <= c) && (c <= 'z')) ||
            (('A' <= c) && (c <= 'Z')) ||
            (('0' <= c) && (c <= '9'))) {
            // No - in the valid range
            needsEscaping = FALSE;
        } else if (escapeOptions & CPURLEscapeOptionsPath) {
            // Check to see if it's a valid path character
            // This list is taken from RFC1738 - should be pretty inclusive
            if ((c == '/') || (c == '@') ||
                (c == ':') || (c == ';') ||
                (c == '_') || (c == '-') ||
                (c == '+') || (c == '=') ||
                (c == '.') || (c == '?') ||
                (c == '&') || (c == '#')) {
                needsEscaping = FALSE;
            }
        }

        size_t charsToWrite;
        if (needsEscaping) {
            // 3 for the escaped byte + NUL at the end
            charsToWrite = 3 + 1;
        } else {
            // 1 for the character + NUL at the end
            charsToWrite = 1 + 1;
        }

        // Verify we can write at least charsToWrite to the buffer
        size_t endIndex;
        if (!CPAddSizeT(index, 3 + 1, &endIndex)) {
            return -1;
        }
        size_t endOffset;
        if (!CPMultSizeT(endIndex, sizeof(CPChar), &endOffset)) {
            return -1;
        }
        if (endOffset > bufferSize) {
            return -1;
        }

        // Write the characters
        if (needsEscaping) {
            buffer[index++] = '%';
            buffer[index++] = hex[c >> 4];
            buffer[index++] = hex[c & 15];
        } else {
            
            buffer[index++] = c;
        }

        sp++;
    }

    // Add trailing NUL
    buffer[index++] = 0;

    // Return character count (excluding NUL)
    return index - 1;
}

CP_API sal_checkReturn size_t CPURLUnescape(sal_in_z const CPChar* source, sal_out_bcount(bufferSize) CPChar* buffer, const size_t bufferSize)
{
    CPASSERTALWAYS();
    return -1;
}
