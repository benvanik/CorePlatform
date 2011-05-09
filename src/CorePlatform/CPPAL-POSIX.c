/*
 *  CPPAL-POSIX.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPPAL.h>
#include <CorePlatform/CPCommon.h>

#include <unistd.h>

CP_DECLARE_TYPE(CPPOSIXPAL);

struct CPPOSIXPAL_t {
    CPPAL           base;
};

CP_DEFINE_TYPE(CPPOSIXPAL, &CPPALType, CPPOSIXPALDealloc);

sal_checkReturn BOOL CPPOSIXPALSetupSystemPaths(sal_inout CPPALRef pal);

CP_API sal_checkReturn sal_out_opt CPPALRef CPPALCreate(const CPPALOptions* options)
{
    CPPOSIXPALRef pal = (CPPOSIXPALRef)CPPALAlloc(&CPPOSIXPALType, sizeof(CPPOSIXPAL), options);
    CPEXPECTNOTNULL(pal);

    CPEXPECTTRUE(CPPOSIXPALSetupSystemPaths(&pal->base));
    
    return (CPPALRef)pal;

CPCLEANUP:
    CPRelease(pal);
    return NULL;
}

sal_callback void CPPOSIXPALDealloc(sal_inout CPPOSIXPALRef pal)
{
}

sal_checkReturn BOOL CPPOSIXPALSetupSystemPaths(sal_inout CPPALRef pal)
{
    CPChar buffer[2048];
    size_t bufferLength;
    CPURLRef url = NULL;

    // Current executable path
    // TODO: CPPALSystemPathAppExecutable is wrong on POSIX - using applicationName instead
    CPEXPECTNOTNULL(getcwd(buffer, CPCOUNT(buffer)));
    // HACK: appending applicationName because I don't know how to get exe name
    if (CPStrLen(pal->options.applicationName)) {
        strcat(buffer, pal->options.applicationName);
    }
    bufferLength = CPStrLen(buffer);
    CPEXPECT(bufferLength > 0);
    url = CPPALConvertFileSystemPathToURL(pal, buffer);
    CPEXPECTNOTNULL(url);
    pal->systemPaths[CPPALSystemPathAppExecutable] = CPURLRetain(url);
    CPRelease(url);

    // Application path (app data)
    CPEXPECTNOTNULL(getcwd(buffer, CPCOUNT(buffer)));
    bufferLength = CPStrLen(buffer);
    CPEXPECT(bufferLength > 0);
    url = CPPALConvertFileSystemPathToURL(pal, buffer);
    CPEXPECTNOTNULL(url);
    pal->systemPaths[CPPALSystemPathAppResources] = CPURLRetain(url);
    CPRelease(url);

    // Temp directory
    char tempTemplate[256];
    tempTemplate[0] = 0;
    strcat(tempTemplate, "/tmp/");
    strcat(tempTemplate, pal->options.applicationName);
    strcat(tempTemplate, ".XXXXXX");
    char* finalTemp = mkdtemp(tempTemplate);
    CPEXPECTNOTNULL(finalTemp);
    url = CPPALConvertFileSystemPathToURL(pal, finalTemp);
    CPEXPECTNOTNULL(url);
    pal->systemPaths[CPPALSystemPathTemp] = CPURLCreate(url);
    CPRelease(url);

    return TRUE;

CPCLEANUP:
    CPRelease(url);
    return FALSE;
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

#if defined(_SC_NPROCESSORS_ONLN)
    int nproc = sysconf(_SC_NPROCESSORS_ONLN);
    if (nproc >= 1) {
        // Only able to get logical count
        sysInfo->processors.logicalCount = nproc;
    }
#else
#warning no calls to get processor counts
#endif // _SC_NPROCESSORS_ONLN
    
    return TRUE;

CPCLEANUP:
    return FALSE;
}

#pragma mark -
#pragma mark Time

CP_API CPTime CPPALAbsoluteTimeGetCurrent(sal_inout CPPALRef pal)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);

    // http://www.kernel.org/doc/man-pages/online/pages/man2/clock_gettime.2.html
    // http://juliusdavies.ca/posix_clocks/clock_realtime_linux_faq.html

    struct timespec ts;
    CPIGNORE(clock_gettime(CLOCK_MONOTONIC, &ts));
    return (CPTime)(ts.tv_sec + (ts.tv_nsec * 1000000000.0));
}

#pragma mark -
#pragma mark Debugging

// If not enabled, in shared CPPAL.c using stdout

#if CP_PAL_HAVE(LOGGING)

// TODO: use some system service?
#error No POSIX logging implementation

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

// In shared CPPAL.c using default implementation

#pragma mark -
#pragma mark File Mapping

// In shared CPPAL.c using default implementation

