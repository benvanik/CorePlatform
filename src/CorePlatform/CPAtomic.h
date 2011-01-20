/*
 *  CPAtomic.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CorePlatform/CPPlatform.h>
#include <CorePlatform/CPConfig.h>
#include <CorePlatform/CPTypes.h>
#include <CorePlatform/CPAnnotations.h>
#include <CorePlatform/CPCodeUtilities.h>

// These functions are modeled off of the Apple OSAtomic routines
// http://developer.apple.com/library/mac/#documentation/DriversKernelHardware/Reference/libkern_ref/OSAtomic_h/

#if CP_LIKE(OSX)
#include <libkern/OSAtomic.h>

#define CPAtomicInc32(value)                        OSAtomicIncrement32Barrier(value)
#define CPAtomicDec32(value)                        OSAtomicDecrement32Barrier(value)
#define CPAtomicAdd32(amount, value)                ((void)OSAtomicAdd32Barrier(amount, value))
#define CPAtomicSub32(amount, value)                ((void)OSAtomicAdd32Barrier(-amount, value))
#define CPAtomicCAS32(oldValue, newValue, value)    OSAtomicCompareAndSwap32Barrier(oldValue, newValue, value)

typedef OSQueueHead CPAtomicStack;
#define CPAtomicStackSetup(stack)                   *(stack) = (OSQueueHead)OS_ATOMIC_QUEUE_INIT
#define CPAtomicStackEnqueue(stack, item, offset)   OSAtomicEnqueue((OSQueueHead*)stack, item, offset)
#define CPAtomicStackDequeue(stack, offset)         OSAtomicDequeue((OSQueueHead*)stack, offset)

#elif CP_LIKE(WIN32)

#define CPAtomicInc32(value)                        InterlockedIncrement((volatile LONG*)value)
#define CPAtomicDec32(value)                        InterlockedDecrement((volatile LONG*)value)
#define CPAtomicAdd32(amount, value)                ((void)InterlockedExchangeAdd((volatile LONG*)value, amount))
#define CPAtomicSub32(amount, value)                ((void)InterlockedExchangeSubtract((volatile unsigned*)value, amount))
#define CPAtomicCAS32(oldValue, newValue, value)    (InterlockedCompareExchange((volatile LONG*)value, newValue, oldValue) == oldValue)

typedef SLIST_HEADER CPAtomicStack;
#define CPAtomicStackSetup(stack)                   InitializeSListHead((PSLIST_HEADER)stack)
#define CPAtomicStackEnqueue(stack, item, offset)   CPIGNORE(InterlockedPushEntrySList((PSLIST_HEADER)stack, (PSLIST_ENTRY)((byte*)item + offset)))
CPFORCEINLINE sal_checkReturn sal_out_opt void* CPAtomicStackDequeue(sal_inout CPAtomicStack* stack, const size_t offset)
{
    void* ptr = (void*)InterlockedPopEntrySList((PSLIST_HEADER)stack);
    if (ptr) {
        return (void*)(((byte*)ptr) - offset);
    } else {
        return NULL;
    }
}

//#elif CP_LIKE(POSIX)

#else

#error No atomic primitives defined for this platform/cpu combination

#endif
