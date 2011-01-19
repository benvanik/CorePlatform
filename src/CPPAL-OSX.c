/*
 *  CPPAL-OSX.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CPPAL.h>
#include <CPCommon.h>

#include <CoreFoundation/CoreFoundation.h>

#include <sys/types.h>
#include <sys/sysctl.h>

#include <mach/mach_time.h>
#include <mach/kern_return.h>

#include <syslog.h>
#include <asl.h>

CP_DECLARE_TYPE(CPOSXPAL);

struct CPOSXPAL_t {
    CPPAL           base;

    CPTime          timeToSec;
};

CP_DEFINE_TYPE(CPOSXPAL, &CPPALType, CPOSXPALDealloc);

CP_API sal_checkReturn sal_out_opt CPPALRef CPPALCreate(const CPPALOptions* options)
{
    CPOSXPALRef pal = (CPOSXPALRef)CPPALAlloc(&CPOSXPALType, sizeof(CPOSXPAL), options);
    CPEXPECTNOTNULL(pal);

    mach_timebase_info_data_t info;
    CPEXPECT(mach_timebase_info(&info), KERN_SUCCESS);
    pal->timeToSec = (CPTime)((info.numer / info.denom) / 1000000000.0);
    
    return (CPPALRef)pal;
    
CPCLEANUP:
    CPRelease(pal);
    return NULL;
}

sal_callback void CPOSXPALDealloc(sal_inout CPOSXPALRef pal)
{
}

#pragma mark -
#pragma mark System Info

CP_API BOOL CPPALSystemInfoQuery(sal_inout CPPALRef pal, sal_inout CPPALSystemInfo* sysInfo)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    CPEXPECTTRUE(CPZeroMemory(sysInfo, sizeof(CPPALSystemInfo), 0, sizeof(CPPALSystemInfo)));
    
    sysInfo->processors.physicalCount   = 1;
    sysInfo->processors.logicalCount    = 1;
    
    // Get processor counts
    // http://geekinfo.googlecode.com/svn-history/r74/trunk/src/macosxsystem.cpp
    int count;
    size_t size = sizeof(int);
    if (sysctlbyname("hw.physicalcpu", &count, &size, NULL, 0) == 0) {
        sysInfo->processors.physicalCount = count;
    } else if (sysctlbyname("hw.packages", &count, &size, NULL, 0) == 0) {
        sysInfo->processors.physicalCount = count;
    } else {
        sysInfo->processors.physicalCount = 1;
    }
    if (sysctlbyname("hw.logicalcpu", &count, &size, NULL, 0) == 0) {
        sysInfo->processors.logicalCount = count;
    } else if (sysctlbyname("hw.ncpu", &count, &size, NULL, 0) == 0) {
        sysInfo->processors.logicalCount = count;
    } else {
        sysInfo->processors.logicalCount = 1;
    }
    
    return TRUE;

CPCLEANUP:
    return FALSE;
}

#pragma mark -
#pragma mark Time

CP_API CPTime CPPALAbsoluteTimeGetCurrent(sal_inout CPPALRef pal)
{
    // According to a bunch of random posts, CACurrentMediaTime is a better call:
    // https://devforums.apple.com/message/118806#118806
    // CACurrentMediaTime is based on mach_absolute_time(), which doesn't have a dependency on QuartzCore:
    // http://developer.apple.com/library/mac/#qa/qa2004/qa1398.html

    //return (CPTime)CFAbsoluteTimeGetCurrent();
    //return (CPTime)CACurrentMediaTime();

    CPOSXPAL* osxpal = (CPOSXPAL*)pal;
    return (CPTime)(mach_absolute_time() * osxpal->timeToSec);
}

#pragma mark -
#pragma mark Debugging

// If not enabled, in shared CPPAL.c using stdout

#if CP_PAL_HAVE(LOGGING)

// TODO: use NSLogger or something else?
#error No OSX logging implementation yet

CP_API void CPPALLogMessage(sal_in_z const CPChar* filePath, const int32 lineNumber, sal_in_z const CPChar* functionName, sal_in_z_opt const CPChar* tag, const CPPALLogLevel level, sal_in_z const CPChar* fmt, ...)
{
}

CP_API void CPPALLogData(sal_in_z const CPChar* filePath, const int32 lineNumber, sal_in_z const CPChar* functionName, sal_in_z_opt const CPChar* tag, const CPPALLogLevel level, sal_in_bcount(size) const CPByte* data, const size_t size)
{
}

CP_API void CPPALLogBitmap(sal_in_z const CPChar* filePath, const int32 lineNumber, sal_in_z const CPChar* functionName, sal_in_z_opt const CPChar* tag, const CPPALLogLevel level, const uint32 width, const uint32 height, const uint32 bytesPerPixel, sal_in_bcount(size) const CPByte* bitmap, const size_t size)
{
}

#endif // LOGGING

#pragma mark -
#pragma mark Mutexes

// In shared CPPAL.c using pthreads

#pragma mark -
#pragma mark Conditions

// In shared CPPAL.c using pthreads

#pragma mark -
#pragma mark Heaps

// In shared CPPAL.c using system allocators

#pragma mark -
#pragma mark Thread Management

// In shared CPPAL.c using pthreads

#pragma mark -
#pragma mark Thread Operations

// In shared CPPAL.c using pthreads

#pragma mark -
#pragma mark Thread Blocks

#define CP_DUMMY_THREAD_BLOCK ((void*)0xDEADBEEF)

CP_API sal_out void* CPPALThreadBlockBegin(sal_inout CPPALRef pal)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
#if defined(__OBJC__)
    return (void*)[[NSAutoreleasePool alloc] init];
#else
    return CP_DUMMY_THREAD_BLOCK;
#endif
}

CP_API void CPPALThreadBlockEnd(sal_inout CPPALRef pal, sal_inout void* block)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
#if defined(__OBJC__)
    NSAutoreleasePool* pool = (NSAutoreleasePool*)block;
    [pool drain];
#else
    CPASSERT(block == CP_DUMMY_THREAD_BLOCK);
#endif
}
