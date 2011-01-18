/*
 *  test_array.c
 *  CPUnitTests-OSX
 *
 *  Created by Ben Vanik on 1/17/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "shared.h"

int _test_array_total_item_count = 0;

CP_DECLARE_TYPE(CPArrayItem);
struct CPArrayItem_t {
    CPObject        base;
};
CP_DEFINE_TYPE(CPArrayItem, NULL, CPArrayItemDealloc);
sal_checkReturn sal_out_opt CPArrayItem* CPArrayItemCreate()
{
    CPArrayItem* obj = (CPArrayItem*)CPObjectAlloc(&CPArrayItemType, sizeof(CPArrayItem));
    if (!obj) {
        return NULL;
    }
    _test_array_total_item_count++;
    return obj;
}
sal_callback void CPArrayItemDealloc(sal_inout CPArrayItem* obj)
{
    _test_array_total_item_count--;
}


static int test_array_init()
{
    return 0;
}

static int test_array_clean()
{
    return 0;
}

void test_array_create()
{
    CPArray* array;
    
    array = CPArrayCreate(0);
    CU_ASSERT(array != NULL);
    CU_ASSERT(CPArrayGetCapacity(array) == 0);
    CU_ASSERT(CPArrayGetCount(array) == 0);
    CPRelease(array);
    
    array = CPArrayCreate(128);
    CU_ASSERT(array != NULL);
    CU_ASSERT(CPArrayGetCapacity(array) == 128);
    CU_ASSERT(CPArrayGetCount(array) == 0);
    CPRelease(array);
    
    array = CPArrayCreate(1024 * 1024);
    CU_ASSERT(array != NULL);
    CU_ASSERT(CPArrayGetCapacity(array) == 1024 * 1024);
    CU_ASSERT(CPArrayGetCount(array) == 0);
    CPRelease(array);
}

void test_array_expansion()
{
    CPArray* array;
    BOOL anyFailed;
    
    array = CPArrayCreate(0);
    CU_ASSERT(array != NULL);
    CU_ASSERT(CPArrayGetCapacity(array) == 0);
    CU_ASSERT(CPArrayGetCount(array) == 0);
    anyFailed = FALSE;
    for (size_t n = 0; n < 128; n++) {
        CPArrayItem* item = CPArrayItemCreate();
        if (!item) {
            anyFailed = TRUE;
            continue;
        }
        if (!CPArrayAddItem(array, item)) {
            anyFailed = TRUE;
        }
        CU_ASSERT(CPArrayGetCapacity(array) >= n + 1);
        CU_ASSERT(CPArrayGetCount(array) == n + 1);
        CPRelease(item);
    }
    CU_ASSERT_FALSE(anyFailed);
    CPRelease(array);
    
    array = CPArrayCreate(128);
    CU_ASSERT(array != NULL);
    CU_ASSERT(CPArrayGetCapacity(array) == 128);
    CU_ASSERT(CPArrayGetCount(array) == 0);
    anyFailed = FALSE;
    for (size_t n = 0; n < 128; n++) {
        CPArrayItem* item = CPArrayItemCreate();
        if (!item) {
            anyFailed = TRUE;
            continue;
        }
        if (!CPArrayAddItem(array, item)) {
            anyFailed = TRUE;
        }
        CU_ASSERT(CPArrayGetCapacity(array) == 128);
        CU_ASSERT(CPArrayGetCount(array) == n + 1);
        CPRelease(item);
    }
    CU_ASSERT_FALSE(anyFailed);
    CU_ASSERT(CPArrayGetCapacity(array) == 128);
    CPRelease(array);
}

void test_array_addremove()
{
    CPArray* array;
    BOOL anyFailed;
    CPArrayItem* item0;
    CPArrayItem* item1;
    CPArrayItem* item2;
    
    _test_array_total_item_count = 0;
    array = CPArrayCreate(128);
    CU_ASSERT(array != NULL);
    anyFailed = FALSE;
    for (size_t n = 0; n < 128; n++) {
        CPArrayItem* item = CPArrayItemCreate();
        if (!item) {
            anyFailed = TRUE;
            continue;
        }
        if (!CPArrayAddItem(array, item)) {
            anyFailed = TRUE;
        }
        CPRelease(item);
    }
    CU_ASSERT_FALSE(anyFailed);
    CU_ASSERT(_test_array_total_item_count == 128);
    CU_ASSERT(CPArrayGetCount(array) == 128);
    CPRelease(array);
    CU_ASSERT(_test_array_total_item_count == 0);
    
    _test_array_total_item_count = 0;
    array = CPArrayCreate(128);
    CU_ASSERT(array != NULL);
    anyFailed = FALSE;
    for (size_t n = 0; n < 128; n++) {
        CPArrayItem* item = CPArrayItemCreate();
        CU_ASSERT(item != NULL);
        if (!CPArrayAddItem(array, item)) {
            anyFailed = TRUE;
        }
        CPRelease(item);
    }
    CU_ASSERT_FALSE(anyFailed);
    CU_ASSERT(_test_array_total_item_count == 128);
    CU_ASSERT(CPArrayGetCount(array) == 128);
    CPRelease(array);
    CU_ASSERT(_test_array_total_item_count == 0);
    
    _test_array_total_item_count = 0;
    array = CPArrayCreate(0);
    CU_ASSERT(array != NULL);
    item0 = CPArrayItemCreate();
    CU_ASSERT(item0 != NULL);
    CU_ASSERT_TRUE(CPArrayAddItem(array, item0));
    item1 = CPArrayItemCreate();
    CU_ASSERT(item1 != NULL);
    CU_ASSERT_TRUE(CPArrayAddItem(array, item1));
    item2 = CPArrayItemCreate();
    CU_ASSERT(item2 != NULL);
    CU_ASSERT_TRUE(CPArrayAddItem(array, item2));
    CU_ASSERT(CPArrayGetCount(array) == 3);
    CU_ASSERT_TRUE(CPArrayRemoveItemAtIndex(array, 2));
    CU_ASSERT(CPArrayGetIndexOfItem(array, item0) == 0);
    CU_ASSERT(CPArrayGetIndexOfItem(array, item1) == 1);
    CU_ASSERT(CPArrayGetIndexOfItem(array, item2) == CPArrayInvalidIndex);
    CU_ASSERT(CPArrayGetCount(array) == 2);
    CPRelease(array);
    CPRelease(item0);
    CPRelease(item1);
    CPRelease(item2);
    CU_ASSERT(_test_array_total_item_count == 0);
    
    _test_array_total_item_count = 0;
    array = CPArrayCreate(0);
    CU_ASSERT(array != NULL);
    item0 = CPArrayItemCreate();
    CU_ASSERT(item0 != NULL);
    CU_ASSERT_TRUE(CPArrayAddItem(array, item0));
    item1 = CPArrayItemCreate();
    CU_ASSERT(item1 != NULL);
    CU_ASSERT_TRUE(CPArrayAddItem(array, item1));
    item2 = CPArrayItemCreate();
    CU_ASSERT(item2 != NULL);
    CU_ASSERT_TRUE(CPArrayAddItem(array, item2));
    CU_ASSERT(CPArrayGetCount(array) == 3);
    CU_ASSERT_TRUE(CPArrayRemoveItemAtIndex(array, 1));
    CU_ASSERT(CPArrayGetIndexOfItem(array, item0) == 0);
    CU_ASSERT(CPArrayGetIndexOfItem(array, item1) == CPArrayInvalidIndex);
    CU_ASSERT(CPArrayGetIndexOfItem(array, item2) == 1);
    CU_ASSERT(CPArrayGetCount(array) == 2);
    CPRelease(array);
    CPRelease(item0);
    CPRelease(item1);
    CPRelease(item2);
    CU_ASSERT(_test_array_total_item_count == 0);
    
    _test_array_total_item_count = 0;
    array = CPArrayCreate(0);
    CU_ASSERT(array != NULL);
    item0 = CPArrayItemCreate();
    CU_ASSERT(item0 != NULL);
    CU_ASSERT_TRUE(CPArrayAddItem(array, item0));
    item1 = CPArrayItemCreate();
    CU_ASSERT(item1 != NULL);
    CU_ASSERT_TRUE(CPArrayAddItem(array, item1));
    item2 = CPArrayItemCreate();
    CU_ASSERT(item2 != NULL);
    CU_ASSERT_TRUE(CPArrayAddItem(array, item2));
    CU_ASSERT(CPArrayGetCount(array) == 3);
    CU_ASSERT_TRUE(CPArrayRemoveItemAtIndex(array, 0));
    CU_ASSERT(CPArrayGetIndexOfItem(array, item0) == CPArrayInvalidIndex);
    CU_ASSERT(CPArrayGetIndexOfItem(array, item1) == 0);
    CU_ASSERT(CPArrayGetIndexOfItem(array, item2) == 1);
    CU_ASSERT(CPArrayGetCount(array) == 2);
    CPRelease(array);
    CPRelease(item0);
    CPRelease(item1);
    CPRelease(item2);
    CU_ASSERT(_test_array_total_item_count == 0);
    
    _test_array_total_item_count = 0;
}

static CU_TestInfo test_array_infos[] = {
    { "test_array_create",          test_array_create           },
    { "test_array_expansion",       test_array_expansion        },
    { "test_array_addremove",       test_array_addremove        },
};

CU_REGISTER_SUITE(test_array);
