/*
 *  CPMemory.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPPlatform.h>
#include <CorePlatform/CPConfig.h>
#include <CorePlatform/CPTypes.h>
#include <CorePlatform/CPAnnotations.h>
#include <CorePlatform/CPCodeUtilities.h>
#include <CorePlatform/CPByteOrder.h>

#include <memory.h>

#if CP_LIKE(WIN32)
// Always use _malloca to reduce risk of blowing the stack
// http://msdn.microsoft.com/en-us/library/5471dc8s.aspx
#include <malloc.h>
#define CPStackAlloc(size)              _malloca(size)
#define CPStackFree(ptr)                do { CPASSERTNOTNULL(ptr); if (ptr) { _freea(ptr); ptr = NULL; } } while(0)
#else
// TODO: implement _malloca?
#define CPStackAlloc(size)              ( (size) ? alloca(size) : alloca(1) )
#define CPStackFree(ptr)                do { CPASSERTNOTNULL(ptr); ptr = NULL; } while(0)
#endif

CP_API sal_checkReturn sal_out_bcount_opt(size) void* CPAlloc(const size_t size);
CP_API sal_checkReturn sal_out_bcount_opt(newSize) void* CPRealloc(sal_inout_bcount_opt(oldSize) void* ptr, const size_t oldSize, const size_t newSize);
CP_API void _CPFreeCore(sal_inout_opt void* ptr);
#define CPFree(ptr)                     do { if (ptr) _CPFreeCore((void*)ptr); ptr = NULL; } while (0)

CP_API sal_checkReturn sal_out_bcount_opt(size * count) void* CPAllocArray(const size_t count, const size_t size);
CP_API void _CPFreeArray(sal_inout_opt void* ptr);
#define CPFreeArray(ptr)                do { if (ptr) _CPFreeArray((void*)ptr); ptr = NULL; } while(0)

CP_API sal_checkReturn sal_out_bcount_opt(size) CPAlignedVoid* CPAllocAligned(const size_t size);
CP_API void _CPFreeAligned(sal_inout_opt CPAlignedVoid* ptr);
#define CPFreeAligned(ptr)              do { if (ptr) _CPFreeAligned((CPAlignedVoid*)ptr); ptr = NULL; } while(0)

CP_API BOOL CPZeroMemory(sal_inout_bcount(size) void* ptr, const size_t size, const size_t offset, const size_t length);
CP_API BOOL CPCopyMemory(sal_out_bcount(destSize) void* dest, const size_t destSize, sal_in_bcount(sourceSize) const void* source, const size_t sourceSize);
