/*
 *  CPLFStack.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 2/20/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CorePlatform/CPPlatform.h>
#include <CorePlatform/CPConfig.h>
#include <CorePlatform/CPTypes.h>
#include <CorePlatform/CPAnnotations.h>
#include <CorePlatform/CPCodeUtilities.h>
#include <CorePlatform/CPMath.h>
#include <CorePlatform/CPAtomic.h>
#include <CorePlatform/CPMemory.h>
#include <CorePlatform/CPDebugging.h>
#include <CorePlatform/CPObject.h>

CP_EXTERNC_BEGIN

CP_DECLARE_TYPE(CPLFStack);

struct CPLFStack_t {
    CPObject        object;
    BOOL            opaquePointers;
    size_t          pointerOffset;
    CPAtomicStack   inner;
};

CP_API sal_checkReturn sal_out_opt CPLFStackRef CPLFStackCreate(const BOOL opaquePointers, const size_t pointerOffset);

CP_API void CPLFStackPush(sal_inout CPLFStackRef stack, sal_inout void* value);
CP_API sal_out_opt void* CPLFStackPop(sal_inout CPLFStackRef stack);

// TODO: atomic stack->queue drain

CP_EXTERNC_END
