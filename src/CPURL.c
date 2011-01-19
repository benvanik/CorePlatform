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

size_t _CPURLMeasureAbsoluteLength(sal_inout CPURLRef url)
{
    return -1;
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
    if (baseURL && CPStrStr(source, CPTEXT("://"))) {
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

    // A little late to do the check, but verify the URL is (somewhat) valid
    CPEXPECTNOT(_CPURLMeasureAbsoluteLength(url), -1);

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
    const size_t length = _CPURLMeasureAbsoluteLength(source);
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
    const size_t length = _CPURLMeasureAbsoluteLength(url);
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
    const size_t length = _CPURLMeasureAbsoluteLength(url);
    if (length == -1) {
        return FALSE;
    }

    const size_t totalBytes = _CPStringGetTotalByteLength(length);
    if (totalBytes < bufferSize) {
        return FALSE;
    }

    // TODO: write absolute string to buffer

    return TRUE;
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
