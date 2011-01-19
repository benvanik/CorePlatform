/*
 *  CPLinkedList.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/18/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "CPLinkedList.h"

CP_DEFINE_TYPE(CPLinkedList, NULL, CPLinkedListDealloc);

CP_API sal_checkReturn sal_out_opt CPLinkedListRef CPLinkedListCreate(const size_t prevPointerOffset, const size_t nextPointerOffset)
{
    CPASSERTNOTZERO(prevPointerOffset);
    CPASSERTNOTZERO(nextPointerOffset);

    CPLinkedListRef list = (CPLinkedListRef)CPObjectAlloc(&CPLinkedListType, sizeof(CPLinkedList));
    CPEXPECTNOTNULL(list);
    
    list->prevPointerOffset = prevPointerOffset;
    list->nextPointerOffset = nextPointerOffset;

    list->count             = 0;
    list->head = list->tail = NULL;
    
    return list;
    
CPCLEANUP:
    CPRelease(list);
    return NULL;
}

sal_callback void CPLinkedListDealloc(sal_inout CPLinkedListRef list)
{
    CPLinkedListRemoveAllItems(list);
}

CP_API size_t CPLinkedListGetCount(sal_inout CPLinkedListRef list)
{
    return list->count;
}

CP_API sal_out_opt cpptr CPLinkedListGetHead(sal_inout CPLinkedListRef list)
{
    return list->head;
}

CP_API sal_out_opt cpptr CPLinkedListGetTail(sal_inout CPLinkedListRef list)
{
    return list->tail;
}

CPFORCEINLINE void CPLinkedListSetPrevPointer(sal_inout CPLinkedListRef list, sal_inout cpptr item, sal_inout_opt cpptr prevItem)
{
    *((cpptr*)(((byte*)item) + list->prevPointerOffset)) = prevItem;
}
CPFORCEINLINE void CPLinkedListSetNextPointer(sal_inout CPLinkedListRef list, sal_inout cpptr item, sal_inout_opt cpptr nextItem)
{
    *((cpptr*)(((byte*)item) + list->nextPointerOffset)) = nextItem;
}

CP_API BOOL CPLinkedListAddItem(sal_inout CPLinkedListRef list, sal_inout cpptr item)
{
    CPASSERTNOTNULL(item);
    if (!item) {
        return FALSE;
    }

    if ((CPLinkedListPrevItem(list, item) != NULL) ||
        (CPLinkedListNextItem(list, item) != NULL) ||
        (list->head == item)) {
        // Already in the list
        return FALSE;
    }
    
    size_t newCount;
    if (!CPAddSizeT(list->count, 1, &newCount)) {
        return FALSE;
    }

    CPIGNORE(CPRetain(item));

    if (list->tail) {
        // Append
        CPLinkedListSetNextPointer(list, list->tail, item);
        CPLinkedListSetPrevPointer(list, item, list->tail);
        list->tail = item;
    } else {
        // First item in the list
        list->head = list->tail = item;
    }

    list->count = newCount;

    return TRUE;
}

CP_API void CPLinkedListRemoveItem(sal_inout CPLinkedListRef list, sal_inout cpptr item)
{
    CPASSERTNOTNULL(item);
    if (!item) {
        return;
    }

    if (list->count == 0) {
        // Removing from an empty list?
        return;
    }

    size_t newCount;
    if (!CPSubSizeT(list->count, 1, &newCount)) {
        // Shouldn't be possible due to check above
        return;
    }

    cpptr prev = CPLinkedListPrevItem(list, item);
    cpptr next = CPLinkedListNextItem(list, item);

    if (prev) {
        CPLinkedListSetNextPointer(list, prev, next);
    } else {
        // At the head
        list->head = next;
        if (next) {
            CPLinkedListSetPrevPointer(list, next, NULL);
        }
    }
    if (next) {
        CPLinkedListSetPrevPointer(list, next, prev);
    } else {
        // At the tail
        list->tail = prev;
        if (prev) {
            CPLinkedListSetNextPointer(list, prev, NULL);
        }
    }

    CPLinkedListSetNextPointer(list, item, NULL);
    CPLinkedListSetPrevPointer(list, item, NULL);

    CPRelease(item);

    list->count = newCount;
}

CP_API void CPLinkedListRemoveAllItems(sal_inout CPLinkedListRef list)
{
    cpptr item = list->head;
    while (item) {
        cpptr next = CPLinkedListNextItem(list, item);
        CPLinkedListSetNextPointer(list, item, NULL);
        CPLinkedListSetPrevPointer(list, item, NULL);
        CPRelease(item);
        item = next;
    }
    list->count = 0;
    list->head = list->tail = NULL;
}
