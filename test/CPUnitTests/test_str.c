/*
 *  test_str.c
 *  CPUnitTests-OSX
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "shared.h"

static int test_str_init()
{
    return 0;
}

static int test_str_clean()
{
    return 0;
}

void test_str_dummy()
{
}

static CU_TestInfo test_str_infos[] = {
    { "test_str_dummy",             test_str_dummy              },
};

CU_REGISTER_SUITE(test_str);
