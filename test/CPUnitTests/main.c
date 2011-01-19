//
//  main.c
//  CPUnitTests
//
//  Created by Ben Vanik on 1/16/11.
//  Copyright 2011 Ben Vanik. All rights reserved.
//

#include "shared.h"
#include <Basic.h>
#include <stdio.h>

#define CU_INVOKE_SUITE(name) \
    extern CU_pSuite name##_register(); \
    if (!name##_register()) { \
        goto CUCLEANUP; \
    }

int main(int argc, char *argv[])
{
    if (CU_initialize_registry() != CUE_SUCCESS) {
        printf("CU_initialize_registry failed");
        return CU_get_error();
    }
    
    CU_INVOKE_SUITE(test_math);
    CU_INVOKE_SUITE(test_atomic);
    CU_INVOKE_SUITE(test_memory);
    CU_INVOKE_SUITE(test_str);
    CU_INVOKE_SUITE(test_object);
    CU_INVOKE_SUITE(test_array);
    CU_INVOKE_SUITE(test_linkedlist);
    CU_INVOKE_SUITE(test_string);
    CU_INVOKE_SUITE(test_pal);
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    CU_cleanup_registry();
    return CU_get_error();
    
CUCLEANUP:
    CU_cleanup_registry();
    return CU_get_error();
}
