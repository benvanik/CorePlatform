/*
 *  CPPAL.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CPPlatform.h>
#include <CPConfig.h>
#include <CPTypes.h>
#include <CPCodeUtilities.h>
#include <CPMath.h>
#include <CPMemory.h>
#include <CPDebugging.h>
#include <CPStr.h>
#include <CPObject.h>

//#define CP_PAL_HAVE_RANDOM          1
#if CP_LIKE(WIN32)
#define CP_PAL_HAVE_THREADS         1
#endif
#if CP_LIKE(OSX)
//#define CP_PAL_HAVE_LOGGING         1
#endif
//#define CP_PAL_HAVE_HEAPS           1
#if CP_LIKE(OSX)
#define CP_PAL_HAVE_THREADBLOCKS    1
#endif

#define CP_PAL_HAVE(NAME)   (defined CP_PAL_HAVE_##NAME && CP_PAL_HAVE_##NAME   )

typedef void CPPALMutex;
typedef void CPPALCondition;
typedef void CPPALHeap;
typedef void CPPALThread;
typedef void CPPALFile;

typedef enum CPPALLogLevel_e {
    CPPALLogLevelError              = 3,
    CPPALLogLevelWarning            = 4,
    CPPALLogLevelInfo               = 6,
    CPPALLogLevelDebug              = 7,
} CPPALLogLevel;

typedef enum CPPALHeapOptions_e {
    CPPALHeapOptionsDefault         = 0x00000000,
    CPPALHeapOptionsLockless        = 0x00000001,
    CPPALHeapOptionsFixedSize       = 0x00000002,
} CPPALHeapOptions;

typedef enum CPPALThreadOptions_e {
    CPPALThreadOptionsDefault       = 0x00000000,
    CPPALThreadOptionsJoinable      = 0x00000001,
    CPPALThreadOptionsSmallStack    = 0x00000002,
} CPPALThreadOptions;

typedef enum CPPALThreadPriority_e {
    CPPALThreadPriorityIdle         = 0,
    CPPALThreadPriorityLow          = 1,
    CPPALThreadPriorityNormal       = 2,
    CPPALThreadPriorityHigh         = 3,
    CPPALThreadPriorityRealtime     = 4,
    
    CPPALThreadPriorityDefault      = CPPALThreadPriorityNormal,
} CPPALThreadPriority;

typedef struct CPPALSystemInfo_t {
    struct {
        uint32          physicalCount;
        uint32          logicalCount;
    } processors;
} CPPALSystemInfo;

typedef struct CPPALOptions_t {
    BOOL                reserved;
} CPPALOptions;

CP_DECLARE_TYPE(CPPAL);

struct CPPAL_t {
    CPObject            base;
    
    const CPPALOptions  options;
};

typedef sal_callback void (*CPPALThreadEntry)(sal_inout CPPALRef pal, sal_inout_opt void* state);

CP_API void CPPALOptionsInitialize(sal_inout CPPALOptions* options);
CP_API sal_checkReturn sal_out_bcount_opt(size) CPPALRef CPPALAlloc(sal_in const CPObjectType* type, const size_t size, const CPPALOptions* options);
CP_API sal_checkReturn sal_out_opt CPPALRef CPPALCreate(const CPPALOptions* options);

CP_API BOOL CPPALSystemInfoQuery(sal_inout CPPALRef pal, sal_inout CPPALSystemInfo* sysInfo);

CP_API CPTime CPPALAbsoluteTimeGetCurrent(sal_inout CPPALRef pal);

CP_API int32 CPPALRandomInt32(sal_inout CPPALRef pal);
CP_API double CPPALRandomDouble(sal_inout CPPALRef pal);

CP_API void CPPALLogMessage(sal_in_z const CPChar* filePath, const int32 lineNumber, sal_in_z const CPChar* functionName, sal_in_z_opt const CPChar* tag, const CPPALLogLevel level, sal_in_z const CPChar* fmt, ...);
CP_API void CPPALLogData(sal_in_z const CPChar* filePath, const int32 lineNumber, sal_in_z const CPChar* functionName, sal_in_z_opt const CPChar* tag, const CPPALLogLevel level, sal_in_bcount(size) const byte* data, const size_t size);
CP_API void CPPALLogBitmap(sal_in_z const CPChar* filePath, const int32 lineNumber, sal_in_z const CPChar* functionName, sal_in_z_opt const CPChar* tag, const CPPALLogLevel level, const uint32 width, const uint32 height, const uint32 bytesPerPixel, sal_in_bcount(size) const byte* bitmap, const size_t size);

CP_API sal_checkReturn sal_out_opt CPPALMutex* CPPALMutexCreate(sal_inout CPPALRef pal);
CP_API void CPPALMutexDestroy(sal_inout CPPALMutex* mutex);
CP_API void CPPALMutexLock(sal_inout CPPALMutex* mutex);
CP_API void CPPALMutexUnlock(sal_inout CPPALMutex* mutex);

CP_API sal_checkReturn sal_out_opt CPPALCondition* CPPALConditionCreate(sal_inout CPPALRef pal);
CP_API void CPPALConditionDestroy(sal_inout CPPALCondition* condition);
CP_API BOOL CPPALConditionWait(sal_inout CPPALCondition* condition, sal_inout CPPALMutex* mutex);
CP_API void CPPALConditionBroadcast(sal_inout CPPALCondition* condition);

CP_API sal_checkReturn sal_out_opt CPPALHeap* CPPALHeapCreate(sal_inout CPPALRef pal, const uint32 options, const size_t initialSize);
CP_API void CPPALHeapDestroy(sal_inout CPPALHeap* heap);
CP_API sal_checkReturn sal_out_bcount_opt(size) void* CPPALHeapAlloc(sal_inout CPPALHeap* heap, const size_t size);
CP_API sal_checkReturn sal_out_bcount_opt(newSize) void* CPPALHeapRealloc(sal_inout CPPALHeap* heap, sal_inout_bcount_opt(oldSize) void* ptr, const size_t oldSize, const size_t newSize);
CP_API void CPPALHeapFree(sal_inout CPPALHeap* heap, sal_inout void* ptr);

CP_API sal_checkReturn sal_out_opt CPPALThread* CPPALThreadCreate(sal_inout CPPALRef pal, const uint32 options, CPPALThreadEntry entry, sal_inout_opt void* state);
CP_API BOOL CPPALThreadJoin(sal_inout CPPALRef pal, sal_inout CPPALThread* thread);
CP_API void CPPALThreadShutdown(sal_inout CPPALRef pal, sal_inout CPPALThread* thread, sal_inout CPPALCondition* condition);
CP_API void CPPALThreadShutdownList(sal_inout CPPALRef pal, sal_inout_ecount(count) CPPALThread** threads, const uint32 count, sal_inout CPPALCondition* condition);

CP_API BOOL CPPALThreadIsMain(sal_inout CPPALRef pal);
CP_API sal_out void* CPPALThreadGetID(sal_inout CPPALRef pal);
CP_API void CPPALThreadSetName(sal_inout CPPALRef pal, sal_in_z const CPChar* name);
CP_API void CPPALThreadSetPriority(sal_inout CPPALRef pal, const CPPALThreadPriority priority);
CP_API void CPPALThreadSleep(sal_inout CPPALRef pal, const CPTime delay);

CP_API sal_out void* CPPALThreadBlockBegin(sal_inout CPPALRef pal);
CP_API void CPPALThreadBlockEnd(sal_inout CPPALRef pal, sal_inout void* block);

// TODO: system paths
// TODO: base directory routines
// TODO: base file routines
// TODO: raw file IO
