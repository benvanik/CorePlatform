/*
 *  CPObject.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "CPObject.h"
#include <CPAtomic.h>
#include <CPDebugging.h>

#define CP_INVALID_COUNT    (int32)(0xFFFFFFFF)

CP_API sal_checkReturn sal_out_bcount_opt(size) CPObjectRef CPObjectAlloc(sal_in const CPObjectType* type, const size_t size)
{
    CPASSERTTRUE(size >= sizeof(CPObject));
    CPASSUME(size >= sizeof(CPObject));
    
    CPObjectRef obj = (CPObjectRef)CPAlloc(size);
    if (!obj) {
        return NULL;
    }
    
    obj->type   = type;
    obj->count  = 1;
    
    return obj;
}

CP_API sal_out_opt cpptr CPRetain(sal_inout_opt cpptr ptr)
{
    if (ptr) {
        CPObjectRef obj = (CPObjectRef)ptr;
        CPASSERT(obj->count != CP_INVALID_COUNT);
        CPAtomicInc32(&obj->count);
    }
    return ptr;
}

CP_API void _CPReleaseCore(sal_inout_opt cpptr ptr)
{
    if (!ptr) {
        return;
    }
    
    CPObjectRef obj = (CPObjectRef)ptr;
    CPASSERT(obj->count != CP_INVALID_COUNT);
    if (CPAtomicDec32(&obj->count) == 0) {
        // Call all type deallocators in sequence
        const CPObjectType* type = obj->type;
        while (type) {
            if (type->dealloc) {
                type->dealloc(obj);
            }
            type = type->parentType;
        }
        
        // NOTE: we preserve type to make debugging easier
        obj->count  = CP_INVALID_COUNT;
        
        CPFree(obj);
    }
}
