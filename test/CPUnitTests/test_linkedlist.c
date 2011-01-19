/*
 *  test_linkedlist.c
 *  CPUnitTests-OSX
 *
 *  Created by Ben Vanik on 1/18/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "shared.h"

int _test_linkedlist_total_item_count = 0;

CP_DECLARE_TYPE(CPLinkedListItem);
struct CPLinkedListItem_t {
    CPObject        base;

    cpptr           prevItem;
    cpptr           nextItem;
};
CP_DEFINE_TYPE(CPLinkedListItem, NULL, CPLinkedListItemDealloc);
sal_checkReturn sal_out_opt CPLinkedListItemRef CPLinkedListItemCreate()
{
    CPLinkedListItemRef obj = (CPLinkedListItemRef)CPObjectAlloc(&CPLinkedListItemType, sizeof(CPLinkedListItem));
    if (!obj) {
        return NULL;
    }
    _test_linkedlist_total_item_count++;
    return obj;
}
sal_callback void CPLinkedListItemDealloc(sal_inout CPLinkedListItemRef obj)
{
    _test_linkedlist_total_item_count--;
    CU_ASSERT(obj->prevItem == NULL);
    CU_ASSERT(obj->nextItem == NULL);
}

static int test_linkedlist_init()
{
    return 0;
}

static int test_linkedlist_clean()
{
    return 0;
}

void test_linkedlist_create()
{
    CPLinkedListRef list;
    
    list = CPLinkedListCreate(offsetof(CPLinkedListItem, prevItem), offsetof(CPLinkedListItem, nextItem));
    CU_ASSERT(list != NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 0);
    CU_ASSERT(CPLinkedListGetHead(list) == NULL);
    CU_ASSERT(CPLinkedListGetTail(list) == NULL);
    CPRelease(list);
}

void test_linkedlist_gets()
{
    CPLinkedListRef list;
    CPLinkedListItemRef item0;
    CPLinkedListItemRef item1;
    CPLinkedListItemRef item2;

    // Add 1 item
    _test_linkedlist_total_item_count = 0;
    list = CPLinkedListCreate(offsetof(CPLinkedListItem, prevItem), offsetof(CPLinkedListItem, nextItem));
    CU_ASSERT(list != NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 0);
    CU_ASSERT(CPLinkedListGetHead(list) == NULL);
    CU_ASSERT(CPLinkedListGetTail(list) == NULL);
    item0 = CPLinkedListItemCreate();
    CU_ASSERT(item0 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 1);
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item0));
    CU_ASSERT(CPLinkedListPrevItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 1);
    CU_ASSERT(CPLinkedListGetHead(list) == item0);
    CU_ASSERT(CPLinkedListGetTail(list) == item0);
    CPLinkedListRemoveItem(list, item0);
    CU_ASSERT(CPLinkedListPrevItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 0);
    CU_ASSERT(CPLinkedListGetHead(list) == NULL);
    CU_ASSERT(CPLinkedListGetTail(list) == NULL);
    CPRelease(item0);
    CPRelease(list);
    CU_ASSERT(_test_linkedlist_total_item_count == 0);
    _test_linkedlist_total_item_count = 0;

    // Add 2 items
    _test_linkedlist_total_item_count = 0;
    list = CPLinkedListCreate(offsetof(CPLinkedListItem, prevItem), offsetof(CPLinkedListItem, nextItem));
    CU_ASSERT(list != NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 0);
    CU_ASSERT(CPLinkedListGetHead(list) == NULL);
    CU_ASSERT(CPLinkedListGetTail(list) == NULL);
    item0 = CPLinkedListItemCreate();
    CU_ASSERT(item0 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 1);
    item1 = CPLinkedListItemCreate();
    CU_ASSERT(item1 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 2);
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item0));
    CU_ASSERT(CPLinkedListPrevItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListPrevItem(list, item1) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item1) == NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 1);
    CU_ASSERT(CPLinkedListGetHead(list) == item0);
    CU_ASSERT(CPLinkedListGetTail(list) == item0);
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item1));
    CU_ASSERT(CPLinkedListPrevItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item0) == item1);
    CU_ASSERT(CPLinkedListPrevItem(list, item1) == item0);
    CU_ASSERT(CPLinkedListNextItem(list, item1) == NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 2);
    CU_ASSERT(CPLinkedListGetHead(list) == item0);
    CU_ASSERT(CPLinkedListGetTail(list) == item1);
    CPLinkedListRemoveItem(list, item1);
    CU_ASSERT(CPLinkedListPrevItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListPrevItem(list, item1) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item1) == NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 1);
    CU_ASSERT(CPLinkedListGetHead(list) == item0);
    CU_ASSERT(CPLinkedListGetTail(list) == item0);
    CPLinkedListRemoveItem(list, item0);
    CU_ASSERT(CPLinkedListPrevItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 0);
    CU_ASSERT(CPLinkedListGetHead(list) == NULL);
    CU_ASSERT(CPLinkedListGetTail(list) == NULL);
    CPRelease(item1);
    CPRelease(item0);
    CPRelease(list);
    CU_ASSERT(_test_linkedlist_total_item_count == 0);
    _test_linkedlist_total_item_count = 0;

    // Add 3 and remove out of order
    _test_linkedlist_total_item_count = 0;
    list = CPLinkedListCreate(offsetof(CPLinkedListItem, prevItem), offsetof(CPLinkedListItem, nextItem));
    CU_ASSERT(list != NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 0);
    CU_ASSERT(CPLinkedListGetHead(list) == NULL);
    CU_ASSERT(CPLinkedListGetTail(list) == NULL);
    item0 = CPLinkedListItemCreate();
    CU_ASSERT(item0 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 1);
    item1 = CPLinkedListItemCreate();
    CU_ASSERT(item1 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 2);
    item2 = CPLinkedListItemCreate();
    CU_ASSERT(item2 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 3);
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item0));
    CU_ASSERT(CPLinkedListPrevItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListPrevItem(list, item1) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item1) == NULL);
    CU_ASSERT(CPLinkedListPrevItem(list, item2) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item2) == NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 1);
    CU_ASSERT(CPLinkedListGetHead(list) == item0);
    CU_ASSERT(CPLinkedListGetTail(list) == item0);
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item1));
    CU_ASSERT(CPLinkedListPrevItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item0) == item1);
    CU_ASSERT(CPLinkedListPrevItem(list, item1) == item0);
    CU_ASSERT(CPLinkedListNextItem(list, item1) == NULL);
    CU_ASSERT(CPLinkedListPrevItem(list, item2) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item2) == NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 2);
    CU_ASSERT(CPLinkedListGetHead(list) == item0);
    CU_ASSERT(CPLinkedListGetTail(list) == item1);
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item2));
    CU_ASSERT(CPLinkedListPrevItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item0) == item1);
    CU_ASSERT(CPLinkedListPrevItem(list, item1) == item0);
    CU_ASSERT(CPLinkedListNextItem(list, item1) == item2);
    CU_ASSERT(CPLinkedListPrevItem(list, item2) == item1);
    CU_ASSERT(CPLinkedListNextItem(list, item2) == NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 3);
    CU_ASSERT(CPLinkedListGetHead(list) == item0);
    CU_ASSERT(CPLinkedListGetTail(list) == item2);
    CPLinkedListRemoveItem(list, item1);
    CU_ASSERT(CPLinkedListPrevItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item0) == item2);
    CU_ASSERT(CPLinkedListPrevItem(list, item1) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item1) == NULL);
    CU_ASSERT(CPLinkedListPrevItem(list, item2) == item0);
    CU_ASSERT(CPLinkedListNextItem(list, item2) == NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 2);
    CU_ASSERT(CPLinkedListGetHead(list) == item0);
    CU_ASSERT(CPLinkedListGetTail(list) == item2);
    CPLinkedListRemoveItem(list, item0);
    CU_ASSERT(CPLinkedListPrevItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item0) == NULL);
    CU_ASSERT(CPLinkedListPrevItem(list, item2) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item2) == NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 1);
    CU_ASSERT(CPLinkedListGetHead(list) == item2);
    CU_ASSERT(CPLinkedListGetTail(list) == item2);
    CPLinkedListRemoveItem(list, item2);
    CU_ASSERT(CPLinkedListPrevItem(list, item2) == NULL);
    CU_ASSERT(CPLinkedListNextItem(list, item2) == NULL);
    CU_ASSERT(CPLinkedListGetCount(list) == 0);
    CU_ASSERT(CPLinkedListGetHead(list) == NULL);
    CU_ASSERT(CPLinkedListGetTail(list) == NULL);
    CPRelease(item2);
    CPRelease(item1);
    CPRelease(item0);
    CPRelease(list);
    CU_ASSERT(_test_linkedlist_total_item_count == 0);
    _test_linkedlist_total_item_count = 0;
}

void test_linkedlist_addremove()
{
    CPLinkedListRef list;
    CPLinkedListItemRef item0;
    CPLinkedListItemRef item1;
    CPLinkedListItemRef item2;

    // Add retains/remove releases
    _test_linkedlist_total_item_count = 0;
    list = CPLinkedListCreate(offsetof(CPLinkedListItem, prevItem), offsetof(CPLinkedListItem, nextItem));
    CU_ASSERT(list != NULL);
    item0 = CPLinkedListItemCreate();
    CU_ASSERT(item0 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 1);
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item0));
    CPRelease(item0);
    CU_ASSERT(_test_linkedlist_total_item_count == 1);
    CPLinkedListRemoveItem(list, CPLinkedListGetHead(list));
    CU_ASSERT(_test_linkedlist_total_item_count == 0);
    CPRelease(list);
    _test_linkedlist_total_item_count = 0;

    // Adding multiple times
    _test_linkedlist_total_item_count = 0;
    list = CPLinkedListCreate(offsetof(CPLinkedListItem, prevItem), offsetof(CPLinkedListItem, nextItem));
    CU_ASSERT(list != NULL);
    item0 = CPLinkedListItemCreate();
    CU_ASSERT(item0 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 1);
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item0));
    CU_ASSERT_FALSE(CPLinkedListAddItem(list, item0));
    CU_ASSERT(_test_linkedlist_total_item_count == 1);
    CPLinkedListRemoveItem(list, item0);
    CPRelease(item0);
    CU_ASSERT(_test_linkedlist_total_item_count == 0);
    CPRelease(list);
    _test_linkedlist_total_item_count = 0;

    // Removing multiple times
    _test_linkedlist_total_item_count = 0;
    list = CPLinkedListCreate(offsetof(CPLinkedListItem, prevItem), offsetof(CPLinkedListItem, nextItem));
    CU_ASSERT(list != NULL);
    item0 = CPLinkedListItemCreate();
    CU_ASSERT(item0 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 1);
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item0));
    CPLinkedListRemoveItem(list, item0);
    CPLinkedListRemoveItem(list, item0);
    CPRelease(item0);
    CU_ASSERT(_test_linkedlist_total_item_count == 0);
    CPRelease(list);
    _test_linkedlist_total_item_count = 0;

    // Remove all
    _test_linkedlist_total_item_count = 0;
    list = CPLinkedListCreate(offsetof(CPLinkedListItem, prevItem), offsetof(CPLinkedListItem, nextItem));
    CU_ASSERT(list != NULL);
    item0 = CPLinkedListItemCreate();
    CU_ASSERT(item0 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 1);
    item1 = CPLinkedListItemCreate();
    CU_ASSERT(item1 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 2);
    item2 = CPLinkedListItemCreate();
    CU_ASSERT(item2 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 3);
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item0));
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item1));
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item2));
    CU_ASSERT(CPLinkedListGetCount(list) == 3);
    CU_ASSERT(CPLinkedListGetHead(list) != NULL);
    CU_ASSERT(CPLinkedListGetTail(list) != NULL);
    CPLinkedListRemoveAllItems(list);
    CU_ASSERT(CPLinkedListGetCount(list) == 0);
    CU_ASSERT(CPLinkedListGetHead(list) == NULL);
    CU_ASSERT(CPLinkedListGetTail(list) == NULL);
    CPRelease(item2);
    CPRelease(item1);
    CPRelease(item0);
    CU_ASSERT(_test_linkedlist_total_item_count == 0);
    CPRelease(list);
    _test_linkedlist_total_item_count = 0;

    // Remove all releases
    _test_linkedlist_total_item_count = 0;
    list = CPLinkedListCreate(offsetof(CPLinkedListItem, prevItem), offsetof(CPLinkedListItem, nextItem));
    CU_ASSERT(list != NULL);
    item0 = CPLinkedListItemCreate();
    CU_ASSERT(item0 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 1);
    item1 = CPLinkedListItemCreate();
    CU_ASSERT(item1 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 2);
    item2 = CPLinkedListItemCreate();
    CU_ASSERT(item2 != NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 3);
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item0));
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item1));
    CU_ASSERT_TRUE(CPLinkedListAddItem(list, item2));
    CU_ASSERT(CPLinkedListGetCount(list) == 3);
    CU_ASSERT(CPLinkedListGetHead(list) != NULL);
    CU_ASSERT(CPLinkedListGetTail(list) != NULL);
    CPRelease(item2);
    CPRelease(item1);
    CPRelease(item0);
    CU_ASSERT(_test_linkedlist_total_item_count == 3);
    CPLinkedListRemoveAllItems(list);
    CU_ASSERT(CPLinkedListGetCount(list) == 0);
    CU_ASSERT(CPLinkedListGetHead(list) == NULL);
    CU_ASSERT(CPLinkedListGetTail(list) == NULL);
    CU_ASSERT(_test_linkedlist_total_item_count == 0);
    CPRelease(list);
    _test_linkedlist_total_item_count = 0;
}

static CU_TestInfo test_linkedlist_infos[] = {
    { "test_linkedlist_create",     test_linkedlist_create      },
    { "test_linkedlist_gets",       test_linkedlist_gets        },
    { "test_linkedlist_addremove",  test_linkedlist_addremove   },
};

CU_REGISTER_SUITE(test_linkedlist);
