/*
 *  CPLinkedList.h
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

CP_DECLARE_TYPE(CPLinkedList);

struct CPLinkedList_t {
    CPObject        object;

    size_t          prevPointerOffset, nextPointerOffset;
    
    size_t          count;
    cpptr           head;
    cpptr           tail;
};

CP_API sal_checkReturn sal_out_opt CPLinkedListRef CPLinkedListCreate(const size_t prevPointerOffset, const size_t nextPointerOffset);

CP_API size_t CPLinkedListGetCount(sal_inout CPLinkedListRef list);
CP_API sal_out_opt cpptr CPLinkedListGetHead(sal_inout CPLinkedListRef list);
CP_API sal_out_opt cpptr CPLinkedListGetTail(sal_inout CPLinkedListRef list);

CP_API BOOL CPLinkedListAddItem(sal_inout CPLinkedListRef list, sal_inout cpptr item);

CP_API void CPLinkedListRemoveItem(sal_inout CPLinkedListRef list, sal_inout cpptr item);
CP_API void CPLinkedListRemoveAllItems(sal_inout CPLinkedListRef list);

CPFORCEINLINE sal_out_opt cpptr CPLinkedListPrevItem(sal_inout CPLinkedListRef list, sal_inout cpptr item)
{
    return *((cpptr*)(((byte*)item) + list->prevPointerOffset));
}
CPFORCEINLINE sal_out_opt cpptr CPLinkedListNextItem(sal_inout CPLinkedListRef list, sal_inout cpptr item)
{
    return *((cpptr*)(((byte*)item) + list->nextPointerOffset));
}
