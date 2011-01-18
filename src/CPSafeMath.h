/*
 *  CPSafeMath.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CPPlatform.h>
#include <CPConfig.h>
#include <CPAnnotations.h>

#include <memory.h>
#include <limits.h>

// These arithmetic functions from the Win32 intsafe.h file - always use them when manipulating size_t to avoid arithmetic errors!
// http://msdn.microsoft.com/en-us/library/ff516460.aspx
// http://blogs.msdn.com/b/michael_howard/archive/2006/02/02/523392.aspx

// The fallback for CPMultSizeT comes from the SafeInt RegMultiply
// http://safeint.codeplex.com/

#define CP_INVALID_SIZE_T       ((size_t)-1)

#if CP_CPU(64BIT)
#if CP_COMPILER(MSVC)
#pragma intrinsic(_umul128)
#define CPMult32x32To64(a, b, low, high)   low = _umul128(a, b, &high)
#elif CP_COMPILER(GNUC)
#if defined(__x86_64__)
#define CPMult32x32To64(a, b, low, high)   __asm__("mulq %3" : "=d" (high), "=a" (low) : "a" (a), "rm" (b) : "cc")
#endif
#endif
#endif /* 64BIT */

CPFORCEINLINE sal_checkReturn BOOL CPAddSizeT(const size_t augend, const size_t addend, sal_out sal_deref_out_range(==, augend + addend) size_t* result)
{
    if ((augend + addend) >= augend) {
        *result = (augend + addend);
        return TRUE;
    } else {
        *result = CP_INVALID_SIZE_T;
        return FALSE;
    }
}

CPFORCEINLINE sal_checkReturn BOOL CPSubSizeT(const size_t minuend, const size_t subtrahend, sal_out sal_deref_out_range(==, minuend - subtrahend) size_t* result)
{
    if (minuend >= subtrahend) {
        *result = (minuend - subtrahend);
        return TRUE;
    } else {
        *result = CP_INVALID_SIZE_T;
        return FALSE;
    }
}

#if CP_CPU(32BIT)
CPFORCEINLINE sal_checkReturn BOOL CPMultSizeT(const size_t multiplicand, const size_t muliplier, sal_out sal_deref_out_range(==, multiplicand * multiplier) size_t *result)
{
    const uint64 result64 = ((uint64)(((uint64)((uint32)(multiplicand))) * ((uint32)(muliplier))));
    if (result64 <= ULONG_MAX) {
        *result = (size_t)result64;
        return TRUE;
    } else {
        *result = CP_INVALID_SIZE_T;
        return FALSE;
    }
}
#else
CPFORCEINLINE sal_checkReturn BOOL CPMultSizeT(const size_t multiplicand, const size_t muliplier, sal_out sal_deref_out_range(==, multiplicand * multiplier) size_t *result)
{
#if defined(CPMult32x32To64)
    uint64 high;
    uint64 low;
    CPMult32x32To64(multiplicand, muliplier, low, high);
    if (high == 0) {
        *result = low;
        return TRUE;
    } else {
        *result = CP_INVALID_SIZE_T;
        return FALSE;
    }
#else
    *result = 0;
    const uint32 aHigh = (uint32)(multiplicand >> 32);
    const uint32 aLow  = (uint32)multiplicand;
    const uint32 bHigh = (uint32)(muliplier >> 32);
    const uint32 bLow  = (uint32)muliplier;
    if (aHigh == 0) {
        if (bHigh != 0) {
            *result = (uint64)aLow * (uint64)bHigh;
        }
    }
    else if (bHigh == 0) {
        if (aHigh != 0) {        
            *result = (uint64)aHigh * (uint64)bLow;
        }
    } else {
        *result = CP_INVALID_SIZE_T;
        return FALSE;
    }
    if (*result != 0) {
        if ((uint32)(*result >> 32) != 0) {
            *result = CP_INVALID_SIZE_T;
            return FALSE;
        }
        *result <<= 32;
        const uint64 tmp = (uint64)aLow * (uint64)bLow;
        *result += tmp;
        if (*result < tmp) {
            *result = CP_INVALID_SIZE_T;
            return FALSE;
        }
        return TRUE;
    }
    *result = (uint64)aLow * (uint64)bLow;
    return TRUE;
#endif // CPMult32x32To64
}
#endif
