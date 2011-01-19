/*
 *  CPString.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/18/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "CPString.h"

CP_DEFINE_TYPE(CPString, NULL, CPStringDealloc);

CPFORCEINLINE sal_out size_t _CPStringGetTotalByteLength(const size_t length)
{
    size_t totalChars;
    size_t totalLength;
    CPEXPECTTRUE(CPAddSizeT(length, 1, &totalChars));
    CPEXPECTTRUE(CPMultSizeT(totalChars, sizeof(CPChar), &totalLength));
    return totalLength;
CPCLEANUP:
    return -1;
}

BOOL CPStringValidateRange(const size_t totalLength, const size_t index, const size_t length, sal_out size_t* prealLength)
{
    // Always set so that worst case it's a no-op
    *prealLength = 0;

    CPEXPECTTRUE(index <= totalLength);

    size_t remainingLength;
    CPEXPECTTRUE(CPSubSizeT(totalLength, index, &remainingLength));

    size_t endIndex;
    CPEXPECTTRUE(CPAddSizeT(index, length, &endIndex));

    size_t realLength;
    CPEXPECTTRUE(CPSubSizeT(endIndex, index, &realLength));

    *prealLength = MIN(remainingLength, realLength);
    return TRUE;

CPCLEANUP:
    return FALSE;
}

// NOTE: this is a utility method and should not be exposed!
sal_checkReturn sal_out_opt CPStringRef _CPStringCreateEmpty(const size_t length)
{
    CPStringRef string = NULL;

    // Nasty, disgustingly safe math that computes:
    // totalSize = sizeof(CPString) + ((length + 1) * sizeof(CPChar))
    const size_t totalValueBytes = _CPStringGetTotalByteLength(length);
    size_t totalSize;
    CPEXPECTTRUE(CPAddSizeT(sizeof(CPString), totalValueBytes, &totalSize));

    // Allocate enough room for the object plus the value
    string = (CPStringRef)CPObjectAlloc(&CPStringType, totalSize);
    CPEXPECTNOTNULL(string);

    string->length  = length;

    // Add the NUL terminator
    string->value[0] = 0;

    return string;

CPCLEANUP:
    CPRelease(string);
    return NULL;
}

CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreate(sal_in_z const CPChar* source)
{
    const size_t length = CPStrLen(source);
    return CPStringCreateWithCharacters(source, length);
}

CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreateWithCharacters(sal_in_ecount(length) const CPChar* source, const size_t length)
{
    CPStringRef string = NULL;

    // Compute how many bytes we should copy
    size_t sourceValueBytes;
    CPEXPECTTRUE(CPMultSizeT(length, sizeof(CPChar), &sourceValueBytes));

    string = _CPStringCreateEmpty(length);
    CPEXPECTNOTNULL(string);

    // Copy in the string (this does not include a NUL terminator)
    CPEXPECTTRUE(CPCopyMemory(string->value, sourceValueBytes, source, sourceValueBytes));

    // Add the NUL terminator
    string->value[length] = 0;

    return string;

CPCLEANUP:
    CPRelease(string);
    return NULL;
}

CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreateWithSubstring(sal_inout CPStringRef source, const size_t index, const size_t length)
{
    // Verify range is valid
    size_t realLength;
    if (!CPStringValidateRange(source->length, index, length, &realLength)) {
        return NULL;
    }
    return CPStringCreateWithCharacters(source->value + index, realLength);
}

CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreateWithFormat(sal_in_z const CPChar* format, ...)
{
    va_list args;
    va_start(args, format);
    CPStringRef result = CPStringCreateWithFormatAndArguments(format, args);
    va_end(args);
    return result;
}

CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreateWithFormatAndArguments(sal_in_z sal_format_string const CPChar* format, va_list args)
{
    CPStringRef string = NULL;

    // Attempt to use a small stack buffer first
    // NOTE: this may still be larger than we need on avg, but shouldn't hurt
    CPChar smallBuffer[512];
    size_t result = CPVSNPrintF(smallBuffer, CPCOUNT(smallBuffer), format, args);
    if ((result != -1) && (result < CPCOUNT(smallBuffer))) {
        // Wrote into small buffer successfully
        return CPStringCreateWithCharacters(smallBuffer, result);
    }

    // NOTE: certain implementations of CPVSNPrintF return -1 for any issues, others return the number of characters that would have
    // been written. Because of that we have to handle both:
#if defined(CPVSCPrintF)
    // This implementation always returns -1, so call CPVSCPrintF to query the length
    result = CPVSCPrintF(format, args);
    if (result == -1) {
        return NULL;
    }
#else
    // This implementation has -1 for error or result >= 0 for something valid
    if (result == -1) {
        return NULL;
    }
#endif // CPVSCPrintF

    // Now we will create an empty string of the desired length that we will write into
    string = _CPStringCreateEmpty(result);
    CPEXPECTNOTNULL(string);

    // Try to write again
    size_t totalChars;
    CPEXPECTTRUE(CPAddSizeT(string->length, 1, &totalChars));
    result = CPVSNPrintF(string->value, totalChars, format, args);
    CPEXPECTTRUE((result != -1) && (result < totalChars));
    
    // Wrote into string successfully!
    return string;

CPCLEANUP:
    CPRelease(string);
    return NULL;
}

CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreateCopy(sal_inout CPStringRef source)
{
    return CPStringCreateWithCharacters(source->value, source->length);
}

CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreateByCombiningStrings(sal_inout CPArrayRef array, sal_in_z_opt const CPChar* separatorString)
{
    CPStringRef string = NULL;

    // Compute the total number of required characters by scanning the input array to calculate str + sep pairings
    const size_t count = CPArrayGetCount(array);
    const size_t separatorLength = separatorString ? CPStrLen(separatorString) : 0;
    size_t totalChars = 0;
    for (size_t n = 0; n < count; n++) {
        CPStringRef source = (CPStringRef)CPArrayGetItem(array, n);
        if (!source) {
            continue;
        }
        if (totalChars) {
            // Adding to existing - account for separator
            CPEXPECTTRUE(CPAddSizeT(totalChars, separatorLength, &totalChars));
        }
        const size_t sourceLength = CPStringGetLength(source);
        CPEXPECTTRUE(CPAddSizeT(totalChars, sourceLength, &totalChars));
    }
    if (!totalChars) {
        return CPStringCreate(CPTEXT(""));
    }

    // Allocate empty string to fill
    string = _CPStringCreateEmpty(totalChars);
    CPEXPECTNOTNULL(string);

    CPChar* p = string->value;
    size_t remainingLength = string->length + 1;
    for (size_t n = 0; n < count; n++) {
        CPStringRef source = (CPStringRef)CPArrayGetItem(array, n);
        if (!source) {
            continue;
        }
        if ((p > string->value) && separatorLength) {
            // Adding to existing - add a separator
            CPEXPECTTRUE(CPStrCat(p, remainingLength, separatorString));
            p += separatorLength;
            remainingLength -= separatorLength;
        }
        const size_t sourceLength = CPStringGetLength(source);
        if (sourceLength) {
            CPEXPECTTRUE(CPStrCat(p, remainingLength, source->value));
            p += sourceLength;
            remainingLength -= sourceLength;
        }
    }

    return string;

CPCLEANUP:
    CPRelease(string);
    return NULL;
}

sal_callback void CPStringDealloc(sal_inout CPStringRef string)
{
    // NOTE: no need to free value as it is part of the string structure
    CPUNREFERENCED(string);

#if defined(CP_DEBUG)
    // Fill the value with garbage to make it easier to spot use-after-free errors
    for (size_t n = 0; n < string->length; n++) {
        string->value[n] = 0x254;
    }
#endif
}

CP_API BOOL CPStringIsNullOrEmpty(sal_inout_opt CPStringRef string)
{
    return !string || !CPStringGetLength(string);
}

CP_API size_t CPStringGetLength(sal_inout CPStringRef string)
{
    return string->length;
}

CP_API sal_out_z const CPChar* CPStringGet(sal_inout CPStringRef string)
{
    return string->value;
}

CP_API sal_checkReturn BOOL CPStringGetCharacters(sal_inout CPStringRef string, sal_out_ecount(bufferLength) CPChar* buffer, const size_t bufferLength, const size_t index, const size_t length)
{
    // Verify range is valid
    size_t realLength;
    CPEXPECTTRUE(CPStringValidateRange(string->length, index, length, &realLength));

    // Compute the total number of bytes to copy
    size_t sourceBytes;
    CPEXPECTTRUE(CPMultSizeT(realLength, sizeof(CPChar), &sourceBytes));

    // Verify output size
    size_t totalBytes;
    CPEXPECTTRUE(CPAddSizeT(sourceBytes, sizeof(CPChar), &totalBytes));
    CPEXPECTTRUE(bufferLength >= totalBytes);

    // Copy in without the trailing NUL
    CPEXPECTTRUE(CPCopyMemory(buffer, bufferLength, string->value + index, sourceBytes));

    // Add trailing NUL
    buffer[realLength] = 0;

    return TRUE;

CPCLEANUP:
    return FALSE;
}

CP_API int32 CPStringCompare(sal_inout CPStringRef string1, sal_inout CPStringRef string2, const uint32 compareOptions)
{
    if (compareOptions & CPStringOptionsCaseInsensitive) {
        return CPStrCaseCmp(string1->value, string2->value);
    } else {
        return CPStrCmp(string1->value, string2->value);
    }
}

CP_API int32 CPStringCompareChar(sal_inout CPStringRef string1, sal_in_z const CPChar* string2, const uint32 compareOptions)
{
    if (compareOptions & CPStringOptionsCaseInsensitive) {
        return CPStrCaseCmp(string1->value, string2);
    } else {
        return CPStrCmp(string1->value, string2);
    }
}

CP_API BOOL CPStringsAreEqual(sal_inout CPStringRef string1, sal_inout CPStringRef string2, const uint32 compareOptions)
{
    // Quick length check
    if (string1->length != string2->length) {
        return FALSE;
    }
    return CPStringCompare(string1, string2, compareOptions) == 0;
}

CP_API BOOL CPStringsAreEqualChar(sal_inout CPStringRef string1, sal_in_z const CPChar* string2, const uint32 compareOptions)
{
    // Quick length check
    const size_t string2Length = CPStrLen(string2);
    if (string1->length != string2Length) {
        return FALSE;
    }
    return CPStringCompareChar(string1, string2, compareOptions) == 0;
}

CP_API BOOL CPStringHasPrefix(sal_inout CPStringRef string, sal_in_z const CPChar* prefix)
{
    if (prefix[0] == 0) {
        return FALSE;
    }
    const size_t prefixLength = CPStrLen(prefix);
    if (prefixLength > string->length) {
        return FALSE;
    } else if (prefixLength == string->length) {
        return CPStringsAreEqualChar(string, prefix, CPStringOptionsCaseSensitive);
    }
    for (size_t n = 0; n < prefixLength; n++) {
        if (string->value[n] != prefix[n]) {
            return FALSE;
        }
    }
    return TRUE;
}

CP_API BOOL CPStringHasSuffix(sal_inout CPStringRef string, sal_in_z const CPChar* suffix)
{
    if (suffix[0] == 0) {
        return FALSE;
    }
    const size_t suffixLength = CPStrLen(suffix);
    if (suffixLength > string->length) {
        return FALSE;
    } else if (suffixLength == string->length) {
        return CPStringsAreEqualChar(string, suffix, CPStringOptionsCaseSensitive);
    }
    size_t startIndex = string->length - suffixLength;
    for (size_t n = 0; n < suffixLength; n++) {
        if (string->value[startIndex + n] != suffix[n]) {
            return FALSE;
        }
    }
    return TRUE;
}

CP_API size_t CPStringFind(sal_inout CPStringRef string, sal_in_z const CPChar* stringToFind)
{
    if (stringToFind[0] == 0) {
        return CPStringInvalidIndex;
    }
    const CPChar* p = CPStrStr(string->value, stringToFind);
    if (p) {
        return (size_t)(p - string->value);
    } else {
        return CPStringInvalidIndex;
    }
}
