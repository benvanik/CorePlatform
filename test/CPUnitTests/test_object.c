/*
 *  test_object.c
 *  CPUnitTests-OSX
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "shared.h"

int _test_object_dealloced          = 0;
int _test_object_base_dealloced     = 0;
int _test_object_subclass_dealloced = 0;

CP_DECLARE_TYPE(CPPlainObject);
struct CPPlainObject_t {
    CPObject        base;
};
CP_DEFINE_TYPE(CPPlainObject, NULL, CPPlainObjectDealloc);
sal_checkReturn sal_out_opt CPPlainObject* CPPlainObjectCreate()
{
    CPPlainObject* obj = (CPPlainObject*)CPObjectAlloc(&CPPlainObjectType, sizeof(CPPlainObject));
    if (!obj) {
        return NULL;
    }
    return obj;
}
sal_callback void CPPlainObjectDealloc(sal_inout CPPlainObject* obj)
{
    _test_object_dealloced = 1;
}

CP_DECLARE_TYPE(CPBaseObject);
struct CPBaseObject_t {
    CPObject        base;
};
CP_DEFINE_TYPE(CPBaseObject, NULL, CPBaseObjectDealloc);
sal_checkReturn sal_out_opt CPBaseObject* CPBaseObjectAlloc(sal_in const CPObjectType* subclassType, const size_t size)
{
    CPBaseObject* obj = (CPBaseObject*)CPObjectAlloc(subclassType, size);
    if (!obj) {
        return NULL;
    }
    return obj;
}
sal_callback void CPBaseObjectDealloc(sal_inout CPBaseObject* obj)
{
    _test_object_base_dealloced = 1;
}

CP_DECLARE_TYPE(CPSubclassObject);
struct CPSubclassObject_t {
    CPBaseObject    base;
};
CP_DEFINE_TYPE(CPSubclassObject, &CPBaseObjectType, CPSubclassObjectDealloc);
sal_checkReturn sal_out_opt CPSubclassObject* CPSubclassObjectCreate()
{
    CPSubclassObject* obj = (CPSubclassObject*)CPBaseObjectAlloc(&CPSubclassObjectType, sizeof(CPSubclassObject));
    if (!obj) {
        return NULL;
    }
    return obj;
}
sal_callback void CPSubclassObjectDealloc(sal_inout CPSubclassObject* obj)
{
    _test_object_subclass_dealloced = 1;
}

static int test_object_init()
{
    return 0;
}

static int test_object_clean()
{
    return 0;
}

void test_object_plain()
{
    _test_object_dealloced = 0;
    
    CPPlainObject* obj = CPPlainObjectCreate();
    CU_ASSERT(obj != NULL);
    CPRelease(obj);
    CU_ASSERT(obj == NULL);
    CU_ASSERT(_test_object_dealloced == 1);
}

void test_object_subclass()
{
    _test_object_base_dealloced = 0;
    _test_object_subclass_dealloced = 0;
    
    CPSubclassObject* obj = (CPSubclassObject*)CPObjectAlloc(&CPSubclassObjectType, sizeof(CPSubclassObject));
    CU_ASSERT(obj != NULL);
    CPRelease(obj);
    CU_ASSERT(obj == NULL);
    CU_ASSERT(_test_object_base_dealloced == 1);
    CU_ASSERT(_test_object_subclass_dealloced == 1);
}

void test_object_retainrelease()
{
    CPPlainObject* obj;
    CPPlainObject* p;
    
    _test_object_dealloced = 0;
    
    obj = CPPlainObjectCreate();
    CU_ASSERT(obj != NULL);
    CU_ASSERT(obj->base.count == 1);
    p = CPPlainObjectRetain(obj);
    CU_ASSERT(p != NULL);
    CU_ASSERT_EQUAL(p, obj);
    CU_ASSERT(obj->base.count == 2);
    CPRelease(p);
    CU_ASSERT(p == NULL);
    CU_ASSERT(obj->base.count == 1);
    CU_ASSERT(_test_object_dealloced == 0);
    CPRelease(obj);
    CU_ASSERT(obj == NULL);
    CU_ASSERT(_test_object_dealloced == 1);
}

static CU_TestInfo test_object_infos[] = {
    { "test_object_plain",          test_object_plain           },
    { "test_object_subclass",       test_object_subclass        },
    { "test_object_retainrelease",  test_object_retainrelease   },
};

CU_REGISTER_SUITE(test_object);
