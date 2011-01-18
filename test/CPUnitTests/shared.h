/*
 *  shared.h
 *  CPUnitTests-OSX
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CUnit.h>
#include <CorePlatform.h>

#define CU_REGISTER_SUITE(name) \
    CU_pSuite name##_register() { \
        CU_pSuite suite = CU_add_suite(#name, name##_init, name##_clean); \
        if (!suite) return NULL; \
        for (int n = 0; n < CPCOUNT(name##_infos); n++) { \
            if (!CU_add_test(suite, name##_infos[n].pName, name##_infos[n].pTestFunc)) { \
                return NULL; \
            } \
        } \
        return suite; \
    }

#define FILL_ZERO(p, off, len) \
    for (int n = 0; n < len; n++) { \
        ((byte*)p)[off + n] = 0; \
    }

#define FILL_DC(p, off, len) \
    for (int n = 0; n < len; n++) { \
        ((byte*)p)[off + n] = 0xDC; \
    }

#define VERIFY_ZEROS(p, off, len) \
    for (int n = 0; n < len; n++) { \
        if (((byte*)p)[off + n] != 0) { \
            CU_ASSERT(((byte*)p)[off + n] == 0); \
            break; \
        } \
    }

#define VERIFY_DC(p, off, len) \
    for (int n = 0; n < len; n++) { \
        if (((byte*)p)[off + n] != 0xDC) { \
            CU_ASSERT(((byte*)p)[off + n] == 0xDC); \
            break; \
        } \
    }
