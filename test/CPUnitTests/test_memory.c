/*
 *  test_memory.c
 *  CPUnitTests-OSX
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "shared.h"

static int test_memory_init()
{
    return 0;
}

static int test_memory_clean()
{
    return 0;
}

void test_memory_stack_alloc_1()
{
    void* x = _malloca(1);
    void* p = CPStackAlloc(0);
    CU_ASSERT(p != NULL);
    CPStackFree(p);
    CU_ASSERT(p == NULL);
}

void test_memory_stack_alloc_2()
{
    void* p = CPStackAlloc(1);
    CU_ASSERT(p != NULL);
    CPStackFree(p);
    CU_ASSERT(p == NULL);
}

void test_memory_stack_alloc_3()
{
    void* p = CPStackAlloc(1024);
    CU_ASSERT(p != NULL);
    CPStackFree(p);
    CU_ASSERT(p == NULL);
}

void test_memory_stack_alloc_4()
{
    void* p = CPStackAlloc(1024 * 1024);
    CU_ASSERT(p != NULL);
    CPStackFree(p);
    CU_ASSERT(p == NULL);
}

void test_memory_stack_alloc_5()
{
    void* p = CPStackAlloc(4 * 1024 * 1024);
    CU_ASSERT(p != NULL);
    CPStackFree(p);
    CU_ASSERT(p == NULL);
}

void test_memory_alloc()
{
    void* p;
    
    p = CPAlloc(0);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 0);
    CPFree(p);
    
    p = CPAlloc(1);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1);
    CPFree(p);
    
    p = CPAlloc(1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1024);
    CPFree(p);
    
    p = CPAlloc(1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1024 * 1024);
    CPFree(p);

    p = CPAlloc(8 * 1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 8 * 1024 * 1024);
    CPFree(p);
}

void test_memory_realloc()
{
    void* p;
    void* o;
    
    p = CPRealloc(NULL, 0, 0);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 0);
    CPFree(p);
    
    p = CPRealloc(NULL, 0, 1);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1);
    CPFree(p);
    
    p = CPRealloc(NULL, 0, 8 * 1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 8 * 1024 * 1024);
    CPFree(p);
    
    p = CPAlloc(0);
    CU_ASSERT(p != NULL);
    p = CPRealloc(p, 0, 0);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 0);
    CPFree(p);
    
    p = CPAlloc(0);
    CU_ASSERT(p != NULL);
    p = CPRealloc(p, 0, 1);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1);
    CPFree(p);
    
    p = CPAlloc(0);
    CU_ASSERT(p != NULL);
    p = CPRealloc(p, 0, 8 * 1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 8 * 1024 * 1024);
    CPFree(p);
    
    p = CPAlloc(1);
    CU_ASSERT(p != NULL);
    p = CPRealloc(p, 1, 0);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 0);
    CPFree(p);
    
    p = CPAlloc(1);
    CU_ASSERT(p != NULL);
    p = CPRealloc(p, 1, 1);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1);
    CPFree(p);
    
    p = CPAlloc(0);
    CU_ASSERT(p != NULL);
    o = p;
    p = CPRealloc(p, 0, 0);
    CU_ASSERT(p != NULL);
    CU_ASSERT(p == o);
    VERIFY_ZEROS(p, 0, 0);
    CPFree(p);
    
    p = CPAlloc(1);
    CU_ASSERT(p != NULL);
    o = p;
    p = CPRealloc(p, 1, 1);
    CU_ASSERT(p != NULL);
    CU_ASSERT(p == o);
    VERIFY_ZEROS(p, 0, 1);
    CPFree(p);
    
    p = CPAlloc(1024);
    CU_ASSERT(p != NULL);
    p = CPRealloc(p, 1024, 1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1024 * 1024);
    CPFree(p);
    
    p = CPAlloc(1024);
    CU_ASSERT(p != NULL);
    FILL_DC(p, 0, 1024);
    p = CPRealloc(p, 1024, 1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_DC(p, 0, 1024);
    VERIFY_ZEROS(p, 1024, 1024 * 1024 - 1024);
    CPFree(p);
}

void test_memory_free()
{
    void* p;
    
    p = NULL;
    CPFree(p);
    CU_ASSERT(p == NULL);
    
    p = CPAlloc(1024);
    CU_ASSERT(p != NULL);
    CPFree(p);
    CU_ASSERT(p == NULL);
}

void test_memory_alloc_array()
{
    void* p;
    
    p = CPAllocArray(0, 0);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 0);
    CPFreeArray(p);
    
    p = CPAllocArray(0, 1);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 0);
    CPFreeArray(p);
    
    p = CPAllocArray(1, 0);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 0);
    CPFreeArray(p);
    
    p = CPAllocArray(1, 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1 * 1024);
    CPFreeArray(p);
    
    p = CPAllocArray(1024, 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1024 * 1024);
    CPFreeArray(p);
    
    p = CPAllocArray(8, 1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 8 * 1024 * 1024);
    CPFreeArray(p);
}

void test_memory_free_array()
{
    void* p;
    
    p = NULL;
    CPFreeArray(p);
    CU_ASSERT(p == NULL);
    
    p = CPAllocArray(2, 1024);
    CU_ASSERT(p != NULL);
    CPFreeArray(p);
    CU_ASSERT(p == NULL);
}

void test_memory_alloc_align()
{
#if CP_CPU(32BIT)
#define ASSERT_ALIGNED(p) CU_ASSERT(((size_t)(p) & 0x3) == 0)
#else
#define ASSERT_ALIGNED(p) CU_ASSERT(((size_t)(p) & 0xF) == 0)
#endif
    
    CPAlignedVoid* p;
    
    p = CPAllocAligned(0);
    CU_ASSERT(p != NULL);
    ASSERT_ALIGNED(p);
    VERIFY_ZEROS(p, 0, 0);
    CPFreeAligned(p);
    
    p = CPAllocAligned(1);
    CU_ASSERT(p != NULL);
    ASSERT_ALIGNED(p);
    VERIFY_ZEROS(p, 0, 1);
    CPFreeAligned(p);
    
    p = CPAllocAligned(4);
    CU_ASSERT(p != NULL);
    ASSERT_ALIGNED(p);
    VERIFY_ZEROS(p, 0, 4);
    CPFreeAligned(p);
    
    p = CPAllocAligned(8);
    CU_ASSERT(p != NULL);
    ASSERT_ALIGNED(p);
    VERIFY_ZEROS(p, 0, 8);
    CPFreeAligned(p);
    
    p = CPAllocAligned(1024);
    CU_ASSERT(p != NULL);
    ASSERT_ALIGNED(p);
    VERIFY_ZEROS(p, 0, 1024);
    CPFreeAligned(p);
    
    p = CPAllocAligned(563);
    CU_ASSERT(p != NULL);
    ASSERT_ALIGNED(p);
    VERIFY_ZEROS(p, 0, 563);
    CPFreeAligned(p);

    p = CPAllocAligned(1248189);
    CU_ASSERT(p != NULL);
    ASSERT_ALIGNED(p);
    VERIFY_ZEROS(p, 0, 1248189);
    CPFreeAligned(p);
}

void test_memory_free_align()
{
    CPAlignedVoid* p;
    
    p = NULL;
    CPFreeAligned(p);
    CU_ASSERT(p == NULL);
    
    p = CPAllocAligned(1024);
    CU_ASSERT(p != NULL);
    CPFreeAligned(p);
    CU_ASSERT(p == NULL);
}

void test_memory_zeromemory()
{
    void* p;
    
    p = NULL;
    CU_ASSERT_TRUE(CPZeroMemory(p, 0, 0, 0));
    
    p = CPAlloc(0);
    CU_ASSERT_TRUE(CPZeroMemory(p, 0, 0, 0));
    CPFree(p);
    
    p = CPAlloc(1024);
    CU_ASSERT(p != NULL);
    FILL_DC(p, 0, 1024);
    CU_ASSERT_TRUE(CPZeroMemory(p, 1024, 0, 1024));
    VERIFY_ZEROS(p, 0, 1024);
    CPFree(p);
    
    p = CPAlloc(1024);
    CU_ASSERT(p != NULL);
    FILL_DC(p, 0, 1024);
    CU_ASSERT_TRUE(CPZeroMemory(p, 1024, 1024, 0));
    VERIFY_DC(p, 0, 1024);
    CPFree(p);
    
    p = CPAlloc(1024);
    CU_ASSERT(p != NULL);
    FILL_DC(p, 0, 1024);
    CU_ASSERT_TRUE(CPZeroMemory(p, 1024, 0, 100));
    VERIFY_ZEROS(p, 0, 100);
    VERIFY_DC(p, 100, 1024 - 100);
    CPFree(p);
    
    p = CPAlloc(1024);
    CU_ASSERT(p != NULL);
    FILL_DC(p, 0, 1024);
    CU_ASSERT_TRUE(CPZeroMemory(p, 1024, 1024 - 100, 100));
    VERIFY_ZEROS(p, 1024 - 100, 100);
    VERIFY_DC(p, 0, 1024 - 100);
    CPFree(p);
    
    p = CPAlloc(1024);
    CU_ASSERT_FALSE(CPZeroMemory(p, 0, 0, 1));
    CU_ASSERT_FALSE(CPZeroMemory(p, 0, 1, 1));
    CU_ASSERT_FALSE(CPZeroMemory(p, 1024, 0, 1025));
    CU_ASSERT_FALSE(CPZeroMemory(p, 1024, 1, 1024));
    CU_ASSERT_FALSE(CPZeroMemory(p, 1024, 1024, 1));
    CU_ASSERT_FALSE(CPZeroMemory(p, 1024, 1000, 1000));
    CPFree(p);
}

void test_memory_copymemory()
{
    void* s;
    void* d;
    
    s = NULL;
    d = NULL;
    CU_ASSERT_TRUE(CPCopyMemory(d, 0, s, 0));
    
    s = NULL;
    d = CPAlloc(1024);
    CU_ASSERT_TRUE(CPCopyMemory(d, 1024, s, 0));
    CPFree(d);
    
    s = CPAlloc(1024);
    d = CPAlloc(1024);
    CU_ASSERT_TRUE(CPCopyMemory(d, 1024, s, 0));
    CPFree(d);
    CPFree(s);
    
    s = CPAlloc(1024);
    d = CPAlloc(1024);
    FILL_DC(s, 0, 1024);
    FILL_ZERO(d, 0, 1024);
    CU_ASSERT_TRUE(CPCopyMemory(d, 1024, s, 1024));
    VERIFY_DC(d, 0, 1024);
    CPFree(d);
    CPFree(s);
    
    s = CPAlloc(1024);
    d = NULL;
    FILL_DC(s, 0, 1024);
    CU_ASSERT_FALSE(CPCopyMemory(d, 0, s, 1024));
    CU_ASSERT_FALSE(CPCopyMemory(d, 1024, s, 1024));
    CPFree(s);
    
    s = CPAlloc(1024);
    d = CPAlloc(1024);
    FILL_DC(s, 0, 1024);
    FILL_ZERO(d, 0, 1024);
    CU_ASSERT_FALSE(CPCopyMemory(d, 0, s, 1024));
    CPFree(s);
}

static CU_TestInfo test_memory_infos[] = {
    { "test_memory_stack_alloc_1",  test_memory_stack_alloc_1   },
    { "test_memory_stack_alloc_2",  test_memory_stack_alloc_2   },
    { "test_memory_stack_alloc_3",  test_memory_stack_alloc_3   },
    { "test_memory_stack_alloc_4",  test_memory_stack_alloc_4   },
    { "test_memory_stack_alloc_5",  test_memory_stack_alloc_5   },
    { "test_memory_alloc",          test_memory_alloc           },
    { "test_memory_realloc",        test_memory_realloc         },
    { "test_memory_free",           test_memory_free            },
    { "test_memory_alloc_array",    test_memory_alloc_array     },
    { "test_memory_free_array",     test_memory_free_array      },
    { "test_memory_alloc_align",    test_memory_alloc_align     },
    { "test_memory_free_align",     test_memory_free_align      },
    { "test_memory_zeromemory",     test_memory_zeromemory      },
    { "test_memory_copymemory",     test_memory_copymemory      },
};

CU_REGISTER_SUITE(test_memory);
