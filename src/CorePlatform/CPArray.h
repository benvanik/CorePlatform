/*
 *  CPArray.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/17/11.
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

#define CPArrayInvalidIndex ((size_t)-1)

CP_DECLARE_TYPE(CPArray);

struct CPArray_t {
    CPObject        object;
    
    size_t          capacity;
    size_t          count;
    sal_field_ecount_part(capacity, count) cpptr* items;
};

CP_API sal_checkReturn sal_out_opt CPArrayRef CPArrayCreate(const size_t initialCapacity);

CP_API size_t CPArrayGetCapacity(sal_inout CPArrayRef array);
CP_API size_t CPArrayGetCount(sal_inout CPArrayRef array);

CP_API BOOL CPArrayExpand(sal_inout CPArrayRef array, const size_t newCapacity);

CP_API sal_out_ecount(*outCount) const cpptr* CPArrayGetHeadPointer(sal_inout CPArrayRef array, sal_deref_out size_t* outCount);

CP_API sal_out_opt cpptr CPArrayGetItem(sal_inout CPArrayRef array, const size_t index);
CP_API size_t CPArrayGetIndexOfItem(sal_inout CPArrayRef array, sal_inout cpptr item);

CP_API BOOL CPArrayAddItem(sal_inout CPArrayRef array, sal_inout cpptr item);

CP_API BOOL CPArrayRemoveItem(sal_inout CPArrayRef array, sal_inout cpptr item);
CP_API BOOL CPArrayRemoveItemAtIndex(sal_inout CPArrayRef array, const size_t index);
CP_API void CPArrayRemoveAllItems(sal_inout CPArrayRef array);

CP_EXTERNC_END
