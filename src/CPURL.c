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
