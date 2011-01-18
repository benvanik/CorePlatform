/*
 *  CPObject.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CPPlatform.h>
#include <CPConfig.h>
#include <CPTypes.h>
#include <CPAnnotations.h>
#include <CPCodeUtilities.h>
#include <CPStr.h>

struct CPObject_t;

typedef void (*CPObjectDealloc)(sal_inout struct CPObject_t* obj);

typedef struct CPObjectType_t {
    const CPChar*                   name;
    const struct CPObjectType_t*    parentType;
    CPObjectDealloc                 dealloc;
} CPObjectType;

#define CP_DECLARE_TYPE(name) \
    struct name##_t; \
    typedef struct name##_t name; \
    typedef struct name##_t* name##Ref; \
    extern const CPObjectType name##Type; \
    CPFORCEINLINE sal_out_opt name##Ref name##Retain(sal_inout_opt name##Ref obj) { \
        return (name##Ref)CPRetain(obj); \
    } \
    CPFORCEINLINE void __dummy_declare_##name##Type() { CPUNREFERENCED(name##Type); }

#define CP_DEFINE_TYPE(name, parentType, dealloc) \
    void dealloc(sal_inout name* obj); \
    const CPObjectType name##Type = {CPTEXT(#name), parentType, (CPObjectDealloc)dealloc}; \
    void __dummy_define_##name##Type() { CPUNREFERENCED(name##Type); }

typedef struct CPObject_t {
    const CPObjectType*     type;
    volatile int32          count;
} CPObject;

typedef struct CPObject_t* CPObjectRef;

#define cpptr void*

CP_API sal_checkReturn sal_out_bcount_opt(size) CPObjectRef CPObjectAlloc(sal_in const CPObjectType* type, const size_t size);
CP_API sal_out_opt cpptr CPRetain(sal_inout_opt cpptr ptr);
CP_API void _CPReleaseCore(sal_inout_opt cpptr ptr);
#define CPRelease(ptr)                  do { if (ptr) _CPReleaseCore((void*)ptr); ptr = NULL; } while (0)
