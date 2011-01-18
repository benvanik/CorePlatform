/*
 *  test_math.c
 *  CPUnitTests-OSX
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "shared.h"

#define MAX_SIZE_T  CP_INVALID_SIZE_T

static int test_math_init()
{
    return 0;
}

static int test_math_clean()
{
    return 0;
}

void test_math_addsizet()
{
    size_t result;
    
    result = 0xDEADBEEF;
    CU_ASSERT(CPAddSizeT(0, 0, &result));
    CU_ASSERT_EQUAL(result, 0);
    
    result = 0xDEADBEEF;
    CU_ASSERT(CPAddSizeT(1, 1, &result));
    CU_ASSERT_EQUAL(result, 2);
    
    result = 0xDEADBEEF;
    CU_ASSERT(!CPAddSizeT(MAX_SIZE_T, 1, &result));
    CU_ASSERT_EQUAL(result, CP_INVALID_SIZE_T);
    
    result = 0xDEADBEEF;
    CU_ASSERT(!CPAddSizeT(MAX_SIZE_T, MAX_SIZE_T, &result));
    CU_ASSERT_EQUAL(result, CP_INVALID_SIZE_T);
}

void test_math_subsizet()
{
    size_t result;
    
    result = 0xDEADBEEF;
    CU_ASSERT(CPSubSizeT(0, 0, &result));
    CU_ASSERT_EQUAL(result, 0);
    
    result = 0xDEADBEEF;
    CU_ASSERT(CPSubSizeT(2, 1, &result));
    CU_ASSERT_EQUAL(result, 1);
    
    result = 0xDEADBEEF;
    CU_ASSERT(CPSubSizeT(MAX_SIZE_T, MAX_SIZE_T, &result));
    CU_ASSERT_EQUAL(result, 0);
    
    result = 0xDEADBEEF;
    CU_ASSERT(!CPSubSizeT(0, 2, &result));
    CU_ASSERT_EQUAL(result, CP_INVALID_SIZE_T);
    
    result = 0xDEADBEEF;
    CU_ASSERT(!CPSubSizeT(1, 2, &result));
    CU_ASSERT_EQUAL(result, CP_INVALID_SIZE_T);
    
    result = 0xDEADBEEF;
    CU_ASSERT(!CPSubSizeT(1, MAX_SIZE_T, &result));
    CU_ASSERT_EQUAL(result, CP_INVALID_SIZE_T);
    
    result = 0xDEADBEEF;
    CU_ASSERT(!CPSubSizeT(MAX_SIZE_T - 1, MAX_SIZE_T, &result));
    CU_ASSERT_EQUAL(result, CP_INVALID_SIZE_T);
}

void test_math_multsizet()
{
    size_t result;
    
    result = 0xDEADBEEF;
    CU_ASSERT(CPMultSizeT(0, 0, &result));
    CU_ASSERT_EQUAL(result, 0);
    
    result = 0xDEADBEEF;
    CU_ASSERT(CPMultSizeT(0, 1, &result));
    CU_ASSERT_EQUAL(result, 0);
    
    result = 0xDEADBEEF;
    CU_ASSERT(CPMultSizeT(1, 0, &result));
    CU_ASSERT_EQUAL(result, 0);
    
    result = 0xDEADBEEF;
    CU_ASSERT(CPMultSizeT(1, 1, &result));
    CU_ASSERT_EQUAL(result, 1);
    
    result = 0xDEADBEEF;
    CU_ASSERT(CPMultSizeT(10, 5, &result));
    CU_ASSERT_EQUAL(result, 50);
    
    result = 0xDEADBEEF;
    CU_ASSERT(CPMultSizeT(1, MAX_SIZE_T, &result));
    CU_ASSERT_EQUAL(result, MAX_SIZE_T);
    
    result = 0xDEADBEEF;
    CU_ASSERT(CPMultSizeT(MAX_SIZE_T, 1, &result));
    CU_ASSERT_EQUAL(result, MAX_SIZE_T);
    
    result = 0xDEADBEEF;
    CU_ASSERT(!CPMultSizeT(MAX_SIZE_T, 2, &result));
    CU_ASSERT_EQUAL(result, CP_INVALID_SIZE_T);
    
    result = 0xDEADBEEF;
    CU_ASSERT(!CPMultSizeT(2, MAX_SIZE_T, &result));
    CU_ASSERT_EQUAL(result, CP_INVALID_SIZE_T);
    
    result = 0xDEADBEEF;
    CU_ASSERT(!CPMultSizeT(MAX_SIZE_T / 2, MAX_SIZE_T / 2, &result));
    CU_ASSERT_EQUAL(result, CP_INVALID_SIZE_T);
}

static CU_TestInfo test_math_infos[] = {
    { "test_math_addsizet",         test_math_addsizet          },
    { "test_math_subsizet",         test_math_subsizet          },
    { "test_math_multsizet",        test_math_multsizet         },
};

CU_REGISTER_SUITE(test_math);
