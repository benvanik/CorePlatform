/*
 *  CPString.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/18/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CPPlatform.h>
#include <CPConfig.h>
#include <CPTypes.h>
#include <CPAnnotations.h>
#include <CPCodeUtilities.h>
#include <CPMath.h>
#include <CPAtomic.h>
#include <CPMemory.h>
#include <CPDebugging.h>
#include <CPObject.h>
#include <CPArray.h>

// CPString is immutable so it is allocated as the CPString object + the character array value
// GCC is fine with a zero sized array, but MSVC doesn't like it (hence the warning junk)
// Design taken largely from CFString: http://developer.apple.com/library/ios/#documentation/CoreFoundation/Reference/CFStringRef/Reference/reference.html

#define CPStringInvalidIndex ((size_t)-1)

typedef enum CPStringOptions_e {
    CPStringOptionsCaseSensitive    = 0x00000000,
    CPStringOptionsCaseInsensitive  = 0x00000001,
} CPStringOptions;

CP_DECLARE_TYPE(CPString);

struct CPString_t {
    CPObject        object;
    
    size_t          length;
    
#pragma warning(push)
#pragma warning(disable : 4200)
    sal_field_ecount_full(length + 1) CPChar value[0];
#pragma warning(pop)
};

CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreate(sal_in_z const CPChar* source);
CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreateWithCharacters(sal_in_ecount(length) const CPChar* source, const size_t length);
CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreateWithSubstring(sal_inout CPStringRef source, const size_t index, const size_t length);
CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreateWithFormat(sal_in_z const CPChar* format, ...);
CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreateWithFormatAndArguments(sal_in_z sal_format_string const CPChar* format, va_list args);
CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreateCopy(sal_inout CPStringRef source);
CP_API sal_checkReturn sal_out_opt CPStringRef CPStringCreateByCombiningStrings(sal_inout CPArrayRef array, sal_in_z_opt const CPChar* separatorString);

CP_API BOOL CPStringIsNullOrEmpty(sal_inout_opt CPStringRef string);
CP_API size_t CPStringGetLength(sal_inout CPStringRef string);
CP_API sal_out_z const CPChar* CPStringGet(sal_inout CPStringRef string);
CP_API sal_checkReturn BOOL CPStringGetCharacters(sal_inout CPStringRef string, sal_out_ecount(bufferLength) CPChar* buffer, const size_t bufferLength, const size_t index, const size_t length);

CP_API int32 CPStringCompare(sal_inout CPStringRef string1, sal_inout CPStringRef string2, const uint32 compareOptions);
CP_API int32 CPStringCompareChar(sal_inout CPStringRef string1, sal_in_z const CPChar* string2, const uint32 compareOptions);
CP_API BOOL CPStringsAreEqual(sal_inout CPStringRef string1, sal_inout CPStringRef string2, const uint32 compareOptions);
CP_API BOOL CPStringsAreEqualChar(sal_inout CPStringRef string1, sal_in_z const CPChar* string2, const uint32 compareOptions);

CP_API BOOL CPStringHasPrefix(sal_inout CPStringRef string, sal_in_z const CPChar* prefix);
CP_API BOOL CPStringHasSuffix(sal_inout CPStringRef string, sal_in_z const CPChar* suffix);
CP_API size_t CPStringFind(sal_inout CPStringRef string, sal_in_z const CPChar* stringToFind);
