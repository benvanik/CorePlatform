/*
 *  test_lfstack.c
 *  CPUnitTests-OSX
 *
 *  Created by Ben Vanik on 2/21/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "shared.h"

int _test_lfstack_total_item_count = 0;

typedef struct CPLFOpaqueItem_t {
    CPStackPointer  lfstack;
    size_t          value;
} CPLFOpaqueItem;

CP_DECLARE_TYPE(CPLFStackItem);
struct CPLFStackItem_t {
    CPObject        base;

    CPStackPointer  lfstack;

    size_t          value;
};
CP_DEFINE_TYPE(CPLFStackItem, NULL, CPLFStackItemDealloc);
sal_checkReturn sal_out_opt CPLFStackItemRef CPLFStackItemCreate(const size_t value)
{
    CPLFStackItemRef obj = (CPLFStackItemRef)CPObjectAlloc(&CPLFStackItemType, sizeof(CPLFStackItem));
    if (!obj) {
        return NULL;
    }
    CPAtomicInc32(&_test_lfstack_total_item_count);
    obj->value = value;
    return obj;
}
sal_callback void CPLFStackItemDealloc(sal_inout CPLFStackItemRef obj)
{
    CPAtomicDec32(&_test_lfstack_total_item_count);
    CU_ASSERT(obj->lfstack == NULL);
}

static int test_lfstack_init()
{
    return 0;
}

static int test_lfstack_clean()
{
    return 0;
}

void test_lfstack_create()
{
    CPLFStackRef stack;

    stack = CPLFStackCreate(TRUE, offsetof(CPLFOpaqueItem, lfstack));
    CU_ASSERT(stack != NULL);
    CU_ASSERT(CPLFStackPop(stack) == NULL);
    CPRelease(stack);

    stack = CPLFStackCreate(FALSE, offsetof(CPLFStackItem, lfstack));
    CU_ASSERT(stack != NULL);
    CU_ASSERT(CPLFStackPop(stack) == NULL);
    CPRelease(stack);
}

void test_lfstack_opaque()
{
    CPLFStackRef stack;

    stack = CPLFStackCreate(TRUE, offsetof(CPLFOpaqueItem, lfstack));
    CU_ASSERT(stack != NULL);
    CU_ASSERT(CPLFStackPop(stack) == NULL);

    for (size_t n = 0; n < 100; n++) {
        CPLFOpaqueItem* item = (CPLFOpaqueItem*)CPAlloc(sizeof(CPLFOpaqueItem));
        item->value = n;
        CPLFStackPush(stack, item);
    }

    size_t count = 0;
    while (TRUE) {
        CPLFOpaqueItem* item = (CPLFOpaqueItem*)CPLFStackPop(stack);
        if (!item) {
            break;
        }
        count++;
        CU_ASSERT(item->value == 100 - count);
        CPFree(item);
    }
    CU_ASSERT(count == 100);
    CU_ASSERT(CPLFStackPop(stack) == NULL);

    CPRelease(stack);
}

void test_lfstack_cpptr()
{
    CPLFStackRef stack;

    _test_lfstack_total_item_count = 0;

    stack = CPLFStackCreate(FALSE, offsetof(CPLFStackItem, lfstack));
    CU_ASSERT(stack != NULL);
    CU_ASSERT(CPLFStackPop(stack) == NULL);

    for (size_t n = 0; n < 100; n++) {
        CPLFStackItemRef item = CPLFStackItemCreate(n);
        CPLFStackPush(stack, item);
        CPRelease(item);
    }
    CU_ASSERT(_test_lfstack_total_item_count == 100);

    size_t count = 0;
    while (TRUE) {
        CPLFStackItemRef item = (CPLFStackItemRef)CPLFStackPop(stack);
        if (!item) {
            break;
        }
        count++;
        CU_ASSERT(item->value == 100 - count);
        CPRelease(item);
    }
    CU_ASSERT(count == 100);
    CU_ASSERT(CPLFStackPop(stack) == NULL);
    CU_ASSERT(_test_lfstack_total_item_count == 0);

    CPRelease(stack);
}

void test_lfstack_dealloc()
{
    CPLFStackRef stack;

    _test_lfstack_total_item_count = 0;

    stack = CPLFStackCreate(FALSE, offsetof(CPLFStackItem, lfstack));
    CU_ASSERT(CPLFStackPop(stack) == NULL);
    for (size_t n = 0; n < 100; n++) {
        CPLFStackItemRef item = CPLFStackItemCreate(n);
        CPLFStackPush(stack, item);
        CPRelease(item);
    }
    CU_ASSERT(_test_lfstack_total_item_count == 100);
    CPRelease(stack);
    CU_ASSERT(_test_lfstack_total_item_count == 0);
}

// TODO: a heavily threaded test

static CU_TestInfo test_lfstack_infos[] = {
    { "test_lfstack_create",        test_lfstack_create         },
    { "test_lfstack_opaque",        test_lfstack_opaque         },
    { "test_lfstack_cpptr",         test_lfstack_cpptr          },
    { "test_lfstack_dealloc",       test_lfstack_dealloc        },
};

CU_REGISTER_SUITE(test_lfstack);
