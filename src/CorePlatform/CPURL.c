/*
 *  CPURL.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/18/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPURL.h>

CP_DEFINE_TYPE(CPURL, NULL, CPURLDealloc);

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

    // Verify space for 2 extra characters (in case we add a / and a NUL)
    size_t extraLength;
    size_t extraSize;
    if (!CPAddSizeT(rootLength, 2, &extraLength) ||
        !CPMultSizeT(extraLength, sizeof(CPChar), &extraSize)) {
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

sal_checkReturn BOOL _CPURLConstructAbsoluteString(sal_inout CPURLRef url, const size_t rootLength, sal_out_bcount(bufferSize) CPChar* buffer, const size_t bufferSize)
{
    const size_t remainingChars = (bufferSize - 1) / sizeof(CPChar);

    if (!url->baseURL && _CPURLIsRooted(url->value)) {
        // Likely a rooted URL, e.g. http://host/path
        return CPStrNCpy(buffer, bufferSize / sizeof(CPChar), url->value, remainingChars);
    } else if (url->baseURL) {
        // Relative URL
        if ((url->value[0] == '?') ||
            (url->value[0] == '#')) {
            // Query or fragment - append only

            // Recursively copy in parent
            if (!_CPURLConstructAbsoluteString(url->baseURL, rootLength, buffer, bufferSize)) {
                return FALSE;
            }

            // Find any existing query/fragment and truncate
            if (url->value[0] == '?') {
                CPChar* p = CPStrRChr(buffer, '?');
                if (p) {
                    *p = 0;
                }
            } else if (url->value[0] == '#') {
                CPChar* p = CPStrRChr(buffer, '#');
                if (p) {
                    *p = 0;
                }
            }

            // Append self
            size_t bufferLength = CPStrLen(buffer);
            size_t remainingBuffer = bufferSize / sizeof(CPChar) - bufferLength;
            return CPStrCat(buffer + bufferLength, remainingBuffer, url->value);
        } else if (url->value[0] == '/') {
            // Absolute path - quickly get the root and add to self
            const size_t baseLength = _CPURLCopyRoot(url, buffer, bufferSize);
            if (baseLength == -1) {
                return FALSE;
            }
            size_t remainingBuffer = bufferSize / sizeof(CPChar) - baseLength;
            // Append self
            return CPStrNCpy(buffer + baseLength, remainingBuffer, url->value, remainingChars);
        } else {
            // Relative path - need to construct entire parent and combine the URLs to check
            
            // Recursively copy in parent
            if (!_CPURLConstructAbsoluteString(url->baseURL, rootLength, buffer, bufferSize)) {
                return FALSE;
            }

            // Search the last / and truncate
            CPChar* p = CPStrRChr(buffer, '/');
            if (p) {
                if ((size_t)(p - buffer) < rootLength) {
                    // No / after root - append one
                    p = buffer + rootLength;
                    *p = '/';
                    p++;
                    *p = 0;
                } else {
                    // Found a trailing / - trunc all after it
                    p++;
                    *p = 0;
                }
            } else {
                // No / at all - append one
                p = buffer;
                while (*p) {
                    p++;
                }
                *p = '/';
                p++;
                *p = 0;
            }

            // Append self
            size_t bufferLength = CPStrLen(buffer);
            size_t remainingBuffer = bufferSize / sizeof(CPChar) - bufferLength;
            return CPStrCat(buffer + bufferLength, remainingBuffer, url->value);
        }
    } else {
        // Not root and no base, so invalid?
        return FALSE;
    }
}

size_t _CPURLMeasureAbsoluteLength(sal_inout CPURLRef url)
{
    // Always verify root
    const size_t rootLength = _CPURLMeasureRootLength(url);
    if (rootLength == -1) {
        return -1;
    }

    // Compute sum of all URLs in chain - that's our max buffer (+ one / per URL and one NUL)
    size_t bufferCapacity = 1;
    CPURLRef baseURL = url;
    while (baseURL) {
        if (!CPAddSizeT(bufferCapacity, 1, &bufferCapacity) ||
            !CPAddSizeT(bufferCapacity, baseURL->length, &bufferCapacity)) {
            return -1;
        }
        baseURL = baseURL->baseURL;
    }
    size_t bufferSize;
    if (!CPMultSizeT(bufferCapacity, sizeof(CPChar), &bufferSize)) {
        return -1;
    }
    CPChar* buffer = (CPChar*)CPStackAlloc(bufferSize);
    if (!buffer) {
        return -1;
    }

    const BOOL result = _CPURLConstructAbsoluteString(url, rootLength, buffer, bufferSize);
    size_t bufferLength = -1;
    if (result) {
        bufferLength = CPStrLen(buffer);
    }
    
    CPStackFree(buffer);

    return bufferLength;
}

// NOTE: this is a utility method and should not be exposed!
sal_checkReturn sal_out_opt CPURLRef _CPURLCreateEmpty(sal_inout_opt CPURLRef baseURL, const size_t length)
{
    CPURLRef url = NULL;

    // Nasty, disgustingly safe math that computes:
    // totalSize = sizeof(CPURL) + ((length + 1) * sizeof(CPChar))
    const size_t totalValueBytes = CPStringGetTotalByteLength(length);
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

    const size_t totalBytes = CPStringGetTotalByteLength(length);
    CPEXPECTTRUE(CPURLGetAbsoluteString(source, url->value, totalBytes));

    // NOTE: no validation is performed because we assume that the source URL could not have been created incorrectly

    url->absoluteLength = url->length;
    CPASSERT(CPStrLen(url->value) == url->length);

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
    CPStringRef string = CPStringCreateEmpty(url->absoluteLength);
    CPEXPECTNOTNULL(string);

    const size_t totalBytes = CPStringGetTotalByteLength(url->absoluteLength);
    CPEXPECTTRUE(CPURLGetAbsoluteString(url, string->value, totalBytes));

    CPASSERT(CPStrLen(string->value) == url->absoluteLength);

    return string;

CPCLEANUP:
    CPRelease(string);
    return NULL;
}

CP_API sal_checkReturn BOOL CPURLGetAbsoluteString(sal_inout CPURLRef url, sal_out_bcount_opt(bufferSize) CPChar* buffer, const size_t bufferSize)
{
    const size_t totalBytes = CPStringGetTotalByteLength(url->absoluteLength);
    if (totalBytes > bufferSize) {
        return FALSE;
    }

    // Verify root
    const size_t rootLength = _CPURLMeasureRootLength(url);
    if (rootLength == -1) {
        return FALSE;
    }

    const BOOL result = _CPURLConstructAbsoluteString(url, rootLength, buffer, bufferSize);
    if (result) {
        CPASSERT(CPStrLen(buffer) == url->absoluteLength);
    }
    return result;
}

CP_API size_t CPURLGetScheme(sal_inout CPURLRef url, sal_out_bcount_opt(bufferSize) CPChar* buffer, const size_t bufferSize)
{
    // Scheme is always in base
    if (url->baseURL) {
        return CPURLGetScheme(url->baseURL, buffer, bufferSize);
    }

    const CPChar* p = CPStrStr(url->value, CPTEXT("://"));
    if (!p) {
        return 0;
    }
    const size_t length = p - url->value;
    if (buffer) {
        if (bufferSize < length + 1) {
            return -1;
        }
        if (!CPStrNCpy(buffer, bufferSize, url->value, length)) {
            return -1;
        }
    }
    return length;
}

CP_API sal_out_opt CPStringRef CPURLCopyScheme(sal_inout CPURLRef url)
{
    // TODO: intern common schemes
    CPChar buffer[64];
    if (!CPURLGetScheme(url, buffer, CPCOUNT(buffer))) {
        return NULL;
    }
    return CPStringCreate(buffer);
}

CP_API size_t CPURLGetLastPathComponent(sal_inout CPURLRef url, sal_out_bcount_opt(bufferSize) CPChar* buffer, const size_t bufferSize)
{
    // May be scheme://host, with no trailing slash or filename
    if (!url->baseURL) {
        const CPChar* slash1 = CPStrStr(url->value, CPTEXT("://"));
        if (!slash1) {
            return 0;
        }
        slash1 += 2; // skip :/, set now at the last /
        const CPChar* slash2 = CPStrRChr(url->value, '/');
        if (slash1 == slash2) {
            // Last trailing slash is the scheme ://
            return 0;
        }
    }

    // Assume last path component always in this url instance
    // This is not the case if this entire instance is a query/hash string - just call up
    if ((url->value[0] == '?') || (url->value[0] == '#')) {
        if (url->baseURL) {
            return CPURLGetLastPathComponent(url->baseURL, buffer, bufferSize);
        } else {
            return 0;
        }
    }

    // Grab last path pointer
    const CPChar* p = url->value;
    const CPChar* lastSlash = CPStrRChr(p, '/');
    if (!lastSlash) {
        // Entire instance is the last path component
    } else {
        // Skip to component
        p = lastSlash + 1;
    }

    // Trim any query or fragment at the end
    const CPChar* q = CPStrRChr(p, '?');
    const CPChar* h = CPStrRChr(p, '#');
    const CPChar* last = NULL;
    if (q && h) {
        if (q < h) {
            last = q;
        } else {
            last = h;
        }
    } else if (q) {
        last = q;
    } else if (h) {
        last = h;
    }

    // Found!
    const size_t length = last ? (last - p) : CPStrLen(p);
    if (buffer) {
        if (!CPStrNCpy(buffer, bufferSize, p, length)) {
            return -1;
        }
    }
    return length;
}

CP_API sal_out_opt CPStringRef CPURLCopyLastPathComponent(sal_inout CPURLRef url)
{
    const size_t requiredLength = CPURLGetLastPathComponent(url, NULL, 0);
    if (requiredLength == -1) {
        return NULL;
    }
    CPStringRef string = CPStringCreateEmpty(requiredLength);
    if (!string) {
        return NULL;
    }
    CPChar* stringBuffer = (CPChar*)CPStringGet(string);
    if (CPURLGetLastPathComponent(url, stringBuffer, requiredLength + 1) == -1) {
        CPRelease(string);
        return NULL;
    }
    return string;
}

CP_API size_t CPURLGetPathExtension(sal_inout CPURLRef url, sal_out_bcount_opt(bufferSize) CPChar* buffer, const size_t bufferSize)
{
    // Get filename
    const size_t requiredLength = CPURLGetLastPathComponent(url, NULL, 0);
    if (requiredLength == -1) {
        return -1;
    }
    const size_t filenameSize = CPStringGetTotalByteLength(requiredLength);
    CPChar* filename = (CPChar*)CPStackAlloc(filenameSize);
    CPEXPECT(CPURLGetLastPathComponent(url, filename, requiredLength + 1) != -1);

    // Find last .
    const CPChar* dot = CPStrRChr(filename, '.');
    CPEXPECTNOTNULL(dot);
    dot++;

    const size_t length = CPStrLen(dot);
    if (buffer) {
        if (!CPStrNCpy(buffer, bufferSize, dot, length)) {
            return -1;
        }
    }

    CPStackFree(filename);
    return length;

CPCLEANUP:
    if (filename) {
        CPStackFree(filename);
    }
    return -1;
}

CP_API sal_out_opt CPStringRef CPURLCopyPathExtension(sal_inout CPURLRef url)
{
    // TODO: intern common extensions

    // Try first with our default buffer size (common)
    CPChar buffer[64];
    if (CPURLGetPathExtension(url, buffer, sizeof(buffer)) != -1) {
        return CPStringCreate(buffer);
    }

    // Dynamic allocation
    const size_t requiredLength = CPURLGetPathExtension(url, NULL, 0);
    if (requiredLength == -1) {
        return NULL;
    }
    CPStringRef string = CPStringCreateEmpty(requiredLength);
    if (!string) {
        return NULL;
    }
    CPChar* stringBuffer = (CPChar*)CPStringGet(string);
    if (CPURLGetPathExtension(url, stringBuffer, requiredLength + 1) == -1) {
        CPRelease(string);
        return NULL;
    }
    return string;
}

CP_API BOOL CPURLIsHTTP(sal_inout CPURLRef url)
{
    CPChar buffer[64];
    if (!CPURLGetScheme(url, buffer, CPCOUNT(buffer))) {
        return FALSE;
    }
    return CPStrCaseCmp(buffer, CPTEXT("http")) == 0;
}

CP_API BOOL CPURLIsHTTPS(sal_inout CPURLRef url)
{
    CPChar buffer[64];
    if (!CPURLGetScheme(url, buffer, CPCOUNT(buffer))) {
        return FALSE;
    }
    return CPStrCaseCmp(buffer, CPTEXT("https")) == 0;
}

CP_API BOOL CPURLIsFile(sal_inout CPURLRef url)
{
    CPChar buffer[64];
    if (!CPURLGetScheme(url, buffer, CPCOUNT(buffer))) {
        return FALSE;
    }
    return CPStrCaseCmp(buffer, CPTEXT("file")) == 0;
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

#if defined(CP_WCHAR)
        // Error out on non-ASCII characters
        if (c > 0x255) {
            return -1;
        }
#endif // CP_WCHAR

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
        if (!CPAddSizeT(index, charsToWrite, &endIndex)) {
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

#define CPURLUnescapeNext() \
    if ((c == 0) || ((idx - 2) >= length)) goto decodeDone; \
    c = c1; c1 = c2; c2 = *sp; sp++; idx++;
#define CPURLUnescapeCheck() \
    if ((size_t)(dp - buffer) + 1 >= bufferSize) return CPInvalidSize;

CP_API sal_checkReturn size_t CPURLUnescape(sal_in_z const CPChar* source, sal_out_bcount(bufferSize) CPChar* buffer, const size_t bufferSize)
{
    const size_t length = CPStrLen(source);

    const CPChar* sp = source;
    CPChar* dp = buffer;
    size_t idx = 0;

    // Prime the chars
    CPChar c = *sp;
    sp++; idx++;
    CPChar c1 = 0;
    CPChar c2 = 0;
    if ((c != 0) && (idx < length)) {
        c1 = *sp;
        sp++; idx++;
        if ((c1 != 0) && (idx < length)) {
            c2 = *sp;
            sp++; idx++;
        }
    }

    // For each (potential) char tripplet, decode
    while ((c != 0) && ((idx - 2) <= length)) {
        if(c == '%') {
            if (isxdigit(c1) && isxdigit(c2)) {
                // Valid %HH sequence found
                CPURLUnescapeCheck();

                // Convert to lower case (inline, instead of calling tolower)
                c1 = ((((c1) >= 'A') && ((c1) <= 'Z')) ? ((c1) - 'A' + 'a') : (c1));
                c2 = ((((c2) >= 'A') && ((c2) <= 'Z')) ? ((c2) - 'A' + 'a') : (c2));
                if (c1 <= '9') {
                    c1 = c1 - '0';
                } else {
                    c1 = c1 - 'a' + 10;
                }
                if (c2 <= '9') {
                    c2 = c2 - '0';
                } else {
                    c2 = c2 - 'a' + 10;
                }
                *dp = 16 * c1 + c2;
                dp++;
                c1 = c2 = 255;
                CPURLUnescapeNext();
                CPURLUnescapeNext();
                CPURLUnescapeNext();
            } else {
                // Invalid or incomplete sequence
                CPURLUnescapeCheck();

                *dp = '%';
                dp++;
                CPURLUnescapeNext();
            }
        } else if (c == '+') {
            CPURLUnescapeCheck();

            *dp = ' ';
            dp++;
            CPURLUnescapeNext();
        } else {
            CPURLUnescapeCheck();

            *dp = c;
            dp++;
            CPURLUnescapeNext();
        }
    }
    
decodeDone:
    dp[ 0 ] = 0;
    return dp - buffer;
}

CP_API sal_out_opt CPStringRef CPURLUnescapeString(sal_inout CPStringRef source)
{
    const CPChar* sourceChars = CPStringGet(source);
    const CPChar* percent = CPStrChr(sourceChars, '%');
    if (!percent) {
        return CPStringRetain(source);
    }

    const size_t bufferSize = CPStringGetTotalByteLength(CPStringGetLength(source));
    CPChar* buffer = (CPChar*)CPStackAlloc(bufferSize);
    CPEXPECTNOTNULL(buffer);

    CPEXPECT(CPURLUnescape(sourceChars, buffer, bufferSize) != -1);

    CPStringRef result = CPStringCreate(buffer);
    CPStackFree(buffer);
    return result;

CPCLEANUP:
    CPStackFree(buffer);
    return NULL;
}

CP_API void CPURLNormalizeSlashes(sal_in_z CPChar* url, const CPChar slashChar)
{
    CPChar* p = url;
    while (*p) {
        if ((*p == '/') || (*p == '\\')) {
            *p = slashChar;
        }
        p++;
    }
}
