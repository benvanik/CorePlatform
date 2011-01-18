/*
 *  test_atomic.c
 *  CPUnitTests-OSX
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "shared.h"

static int test_atomic_init()
{
    return 0;
}

static int test_atomic_clean()
{
    return 0;
}

void test_atomic_inc32()
{
    volatile int v;
    
    v = 0;
    CU_ASSERT_EQUAL(CPAtomicInc32(&v), 1);
    CU_ASSERT_EQUAL(v, 1);
    CU_ASSERT_EQUAL(CPAtomicInc32(&v), 2);
    CU_ASSERT_EQUAL(v, 2);

    v = 0xFFFFFFFE;
    CU_ASSERT_EQUAL(CPAtomicInc32(&v), 0xFFFFFFFF);
    CU_ASSERT_EQUAL(v, 0xFFFFFFFF);
    CU_ASSERT_EQUAL(CPAtomicInc32(&v), 0);
    CU_ASSERT_EQUAL(v, 0);
}

void test_atomic_dec32()
{
    volatile int v;
    
    v = 2;
    CU_ASSERT_EQUAL(CPAtomicDec32(&v), 1);
    CU_ASSERT_EQUAL(v, 1);
    CU_ASSERT_EQUAL(CPAtomicDec32(&v), 0);
    CU_ASSERT_EQUAL(v, 0);
    
    v = 0;
    CU_ASSERT_EQUAL(CPAtomicDec32(&v), 0xFFFFFFFF);
    CU_ASSERT_EQUAL(v, 0xFFFFFFFF);
    CU_ASSERT_EQUAL(CPAtomicDec32(&v), 0xFFFFFFFE);
    CU_ASSERT_EQUAL(v, 0xFFFFFFFE);
}

void test_atomic_add32()
{
    volatile int v;
    
    v = 0;
    CPAtomicAdd32(4, &v);
    CU_ASSERT_EQUAL(v, 4);
    
    v = 3;
    CPAtomicAdd32(4, &v);
    CU_ASSERT_EQUAL(v, 7);
    
    v = UINT_MAX;
    CPAtomicAdd32(0, &v);
    CU_ASSERT_EQUAL(v, UINT_MAX);
    
    v = UINT_MAX;
    CPAtomicAdd32(1, &v);
    CU_ASSERT_EQUAL(v, 0);
}

void test_atomic_sub32()
{
    volatile int v;
    
    v = 4;
    CPAtomicSub32(4, &v);
    CU_ASSERT_EQUAL(v, 0);
    
    v = 7;
    CPAtomicSub32(4, &v);
    CU_ASSERT_EQUAL(v, 3);
    
    v = UINT_MAX;
    CPAtomicSub32(0, &v);
    CU_ASSERT_EQUAL(v, UINT_MAX);
    
    v = 0;
    CPAtomicSub32(1, &v);
    CU_ASSERT_EQUAL(v, UINT_MAX);
}

void test_atomic_cas32()
{
    volatile int v;
    
    v = 0;
    CU_ASSERT_EQUAL(CPAtomicCAS32(0, 1, &v), 1);
    CU_ASSERT_EQUAL(v, 1);
    
    v = 0;
    CU_ASSERT_EQUAL(CPAtomicCAS32(1, 0, &v), 0);
    CU_ASSERT_EQUAL(v, 0);
    
    // TODO: test contention somehow
}

typedef struct test_stack_item_t {
    CPAlignedVoid*      pointer;
} test_stack_item;

void test_atomic_stack()
{
    CPAtomicStack stack;
    void* item;
    
    const size_t offset = offsetof(test_stack_item, pointer);
    
    CPAtomicStackSetup(&stack);
    CU_ASSERT_EQUAL(CPAtomicStackDequeue(&stack, offset), NULL);
    
    CPAtomicStackSetup(&stack);
    item = CPAlloc(sizeof(test_stack_item));
    CPAtomicStackEnqueue(&stack, item, offset);
    CU_ASSERT_EQUAL(CPAtomicStackDequeue(&stack, offset), item);
    CPFree(item);
    
    CPAtomicStackSetup(&stack);
    void* item1 = CPAlloc(sizeof(test_stack_item));
    void* item2 = CPAlloc(sizeof(test_stack_item));
    CPAtomicStackEnqueue(&stack, item1, offset);
    CPAtomicStackEnqueue(&stack, item2, offset);
    CU_ASSERT_EQUAL(CPAtomicStackDequeue(&stack, offset), item2);
    CU_ASSERT_EQUAL(CPAtomicStackDequeue(&stack, offset), item1);
    CPFree(item2);
    CPFree(item1);
    
    // TODO: test contention somehow
}

static CU_TestInfo test_atomic_infos[] = {
    { "test_atomic_inc32",          test_atomic_inc32           },
    { "test_atomic_dec32",          test_atomic_dec32           },
    { "test_atomic_add32",          test_atomic_add32           },
    { "test_atomic_sub32",          test_atomic_sub32           },
    { "test_atomic_cas32",          test_atomic_cas32           },
    { "test_atomic_stack",          test_atomic_stack           },
};

CU_REGISTER_SUITE(test_atomic);
