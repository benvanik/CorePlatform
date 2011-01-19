/*
 *  CPArray.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/17/11.
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

CP_API sal_out_opt cpptr CPArrayGetItem(sal_inout CPArrayRef array, const size_t index);
CP_API size_t CPArrayGetIndexOfItem(sal_inout CPArrayRef array, sal_inout cpptr item);

CP_API BOOL CPArrayAddItem(sal_inout CPArrayRef array, sal_inout cpptr item);

CP_API BOOL CPArrayRemoveItem(sal_inout CPArrayRef array, sal_inout cpptr item);
CP_API BOOL CPArrayRemoveItemAtIndex(sal_inout CPArrayRef array, const size_t index);
CP_API void CPArrayRemoveAllItems(sal_inout CPArrayRef array);
