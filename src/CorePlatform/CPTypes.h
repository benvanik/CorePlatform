/*
 *  CPTypes.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CorePlatform/CPPlatform.h>

#include <limits.h>
#include <stddef.h>
#include <ctype.h>

typedef unsigned char       byte;
typedef signed char         int8;
typedef unsigned char       uint8;
typedef signed short        int16;
typedef unsigned short      uint16;
typedef signed int          int32;
typedef unsigned int        uint32;

#if __LP64__
typedef signed long         int64;
typedef unsigned long       uint64;
#else
typedef signed long long    int64;
typedef unsigned long long  uint64;
#endif // __LP64__

typedef double              CPTime;

#if !defined(OBJC_BOOL_DEFINED)
#if CP_LIKE(OSX)
typedef signed char         BOOL;
#else
typedef signed int          BOOL;
#endif
#endif
#if !defined(TRUE)
#define TRUE                ((BOOL)1)
#define FALSE               ((BOOL)0)
#endif

#if !defined(NULL)
#if defined(__GNUG__)
#define NULL                __null
#elif defined(__cplusplus)
#define NULL                0
#else
#define NULL                ((void*)0)
#endif
#endif // !NULL
