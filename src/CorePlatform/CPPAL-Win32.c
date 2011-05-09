/*
 *  CPPAL-Win32.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPPAL.h>
#include <CorePlatform/CPCommon.h>

#if !CP_PLATFORM(XBOX360)
#include <objbase.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif // XBOX360

CP_DECLARE_TYPE(CPWin32PAL);

struct CPWin32PAL_t {
    CPPAL           base;

    DWORD           mainThreadId;

    BOOL            useHighPerfTimer;
    CPTime          invTicksPerSec;
};

CP_DEFINE_TYPE(CPWin32PAL, &CPPALType, CPWin32PALDealloc);

sal_checkReturn BOOL CPWin32PALSetupSystemPaths(sal_inout CPPALRef pal);

CP_API sal_checkReturn sal_out_opt CPPALRef CPPALCreate(const CPPALOptions* options)
{
    CPWin32PALRef pal = (CPWin32PALRef)CPPALAlloc(&CPWin32PALType, sizeof(CPWin32PAL), options);
    CPEXPECTNOTNULL(pal);
    
    // NOTE: we always assume we start on the main thread - this may not be true, but that's what our definition of 'main thread' is
    pal->mainThreadId   = GetCurrentThreadId();

    // Get QPC timing frequency... hopefully stable over the life of the app
    LARGE_INTEGER ticksPerSec;
    if (QueryPerformanceFrequency(&ticksPerSec)) {
        pal->useHighPerfTimer   = TRUE;
        pal->invTicksPerSec     = 1.0 / (CPTime)ticksPerSec.QuadPart;
    } else {
        pal->useHighPerfTimer   = FALSE;
        CPLOGW(CPTEXT("falling back from high performance timer"));
    }

#if !CP_PLATFORM(XBOX360)
    // Setup COM on the main thread
    // NOTE: this may fail if COM has already been initialized - that's OK
    CPIGNORE(CoInitializeEx(NULL, COINIT_MULTITHREADED));
#endif // XBOX360

    CPEXPECTTRUE(CPWin32PALSetupSystemPaths(&pal->base));
    
    return (CPPALRef)pal;

CPCLEANUP:
    CPRelease(pal);
    return NULL;
}

sal_callback void CPWin32PALDealloc(sal_inout CPWin32PALRef pal)
{
#if !CP_PLATFORM(XBOX360)
    CoUninitialize();
#endif // XBOX360
}

sal_checkReturn BOOL CPWin32PALSetupSystemPaths(sal_inout CPPALRef pal)
{
    CPChar buffer[2048];
    size_t bufferLength;
    CPURLRef url = NULL;

    // Current executable path
    bufferLength = GetModuleFileName(NULL, buffer, CPCOUNT(buffer));
    CPEXPECT(bufferLength > 0);
    url = CPPALConvertFileSystemPathToURL(pal, buffer);
    CPEXPECTNOTNULL(url);
    pal->systemPaths[CPPALSystemPathAppExecutable] = CPURLRetain(url);
    CPRelease(url);

    // Application path (app data)
    bufferLength = GetModuleFileName(NULL, buffer, CPCOUNT(buffer));
    CPEXPECT(bufferLength > 0);
    const CPChar* lastSlash = CPStrRChr(buffer, '\\');
    CPEXPECTNOTNULL(lastSlash);
    bufferLength = (lastSlash - buffer) + 1;
    CPEXPECT(bufferLength + 1 < CPCOUNT(buffer));
    buffer[bufferLength] = 0;
    url = CPPALConvertFileSystemPathToURL(pal, buffer);
    CPEXPECTNOTNULL(url);
    pal->systemPaths[CPPALSystemPathAppResources] = CPURLRetain(url);
    CPRelease(url);

    // Temp directory
    bufferLength = GetTempPath(CPCOUNT(buffer), buffer);
    CPEXPECT(bufferLength > 0);
    // TODO: add random secure string on the end of temp path
    CPEXPECTNOTNULL(CPStrCat(buffer, CPCOUNT(buffer), pal->options.applicationName));
    CPEXPECTNOTNULL(CPStrCat(buffer, CPCOUNT(buffer), CPTEXT(".XXXXXXX\\")));
    url = CPPALConvertFileSystemPathToURL(pal, buffer);
    CPEXPECTNOTNULL(url);
    pal->systemPaths[CPPALSystemPathTemp] = CPURLRetain(url);
    CPRelease(url);

    return TRUE;

CPCLEANUP:
    CPRelease(url);
    return FALSE;
}

#pragma mark -
#pragma mark System Info

#if CP_PLATFORM(WIN32)
// Query the processor info - god this API couldn't be any uglier...
// http://msdn.microsoft.com/en-us/library/ms683194.aspx
DWORD CPWin32PALCountSetBits(ULONG_PTR bitMask) {
    DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
    for (DWORD i = 0; i <= LSHIFT; ++i) {
        bitSetCount += ((bitMask & bitTest) ? 1 : 0);
        bitTest /= 2;
    }
    return bitSetCount;
}
typedef BOOL (WINAPI *LPFN_GLPI)(
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, 
    PDWORD);
BOOL CPWin32PALGetProcessorInfo(sal_inout CPPALSystemInfo* sysInfo)
{
    BOOL result = FALSE;
    HMODULE kernel32 = NULL;
    LPFN_GLPI glpi = NULL;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;

    kernel32 = GetModuleHandle(TEXT("kernel32"));
    CPEXPECTNOTNULL(kernel32);
    glpi = (LPFN_GLPI)GetProcAddress(kernel32, "GetLogicalProcessorInformation");
    CPEXPECTNOTNULL(glpi);

    // Call GetLogicalProcessorInformation to get the buffer length, allocate it, and then call again to fill
    DWORD bufferLength = 0;
    CPEXPECTFALSE(glpi(NULL, &bufferLength));
    CPEXPECT(GetLastError() == ERROR_INSUFFICIENT_BUFFER);
    buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)CPStackAlloc(bufferLength);
    CPEXPECTNOTNULL(buffer);
    CPEXPECTTRUE(glpi(buffer, &bufferLength));
    CPEXPECTNOTZERO(bufferLength);

    sysInfo->processors.physicalCount   = 0;
    sysInfo->processors.logicalCount    = 0;

    for (DWORD n = 0; n < (bufferLength / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)); n++) {
        switch (buffer[n].Relationship) {
            case RelationProcessorPackage:
                sysInfo->processors.physicalCount++;
                break;
            case RelationProcessorCore:
                if (buffer[n].ProcessorCore.Flags == 1) {
                    // Hyperthreaded - # of processors set as bits in ProcessorMask
                    sysInfo->processors.logicalCount += CPWin32PALCountSetBits(buffer[n].ProcessorMask);
                } else {
                    // Non-hyperthreaded - just count as one core
                    sysInfo->processors.logicalCount++;
                }
                break;
        }
    }
    
    sysInfo->processors.physicalCount   = MAX(1, sysInfo->processors.physicalCount);
    sysInfo->processors.logicalCount    = MAX(1, sysInfo->processors.logicalCount);

    result = TRUE;
    // Fall through

CPCLEANUP:
    CPStackFree(buffer);
    if (kernel32) {
        CPIGNORE(FreeLibrary(kernel32));
        kernel32 = NULL;
    }
    return result;
}
#endif // WIN32

CP_API BOOL CPPALSystemInfoQuery(sal_inout CPPALRef pal, sal_inout CPPALSystemInfo* sysInfo)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    CPEXPECTTRUE(CPZeroMemory(sysInfo, sizeof(CPPALSystemInfo), 0, sizeof(CPPALSystemInfo)));
    
    sysInfo->processors.physicalCount   = 1;
    sysInfo->processors.logicalCount    = 1;

    // NOTE: there aren't many universally supported ways to get this value, so try a few
#if CP_PLATFORM(XBOX360)
    // Always the same
    sysInfo->processors.physicalCount   = 3;
    sysInfo->processors.logicalCount    = 6;
#elif CP_PLATFORM(WIN32)
    CPEXPECTTRUE(CPWin32PALGetProcessorInfo(sysInfo));
#else
    // Anything to do?
    sysInfo->processors.physicalCount   = 1;
    sysInfo->processors.logicalCount    = 1;
#endif
    
    return TRUE;

CPCLEANUP:
    return FALSE;
}

#pragma mark -
#pragma mark Path Utilities

CP_API BOOL CPPALConvertURLToFileSystemPath(sal_inout CPPALRef pal, sal_inout CPURLRef url, sal_out_bcount(bufferSize) CPChar* buffer, const size_t bufferSize)
{
#if CP_PLATFORM(XBOX360)
    // TODO: XBOX360 CPPALConvertURLToFileSystemPath
    CPASSERTALWAYS();
    return FALSE;
#else
    // Parse URLs in the form of:
    // file:///C:/My/Long%20Path.txt -> C:\My\Long Path.txt

    BOOL result = FALSE;
    CPStringRef absoluteUrl = NULL;

    CPASSERT(bufferSize >= MAX_PATH);
    if (bufferSize < MAX_PATH) {
        return FALSE;
    }
    if (!CPURLIsFile(url)) {
        return FALSE;
    }
    
    absoluteUrl = CPURLCopyAbsoluteString(url);
    CPEXPECTNOTNULL(absoluteUrl);

    CPChar bufferTemp[CP_MAX_PATH];
    CPEXPECT(CPURLUnescape(CPStringGet(absoluteUrl), bufferTemp, CPCOUNT(bufferTemp)) != -1);

    size_t prefixLength = 0;
    if (CPStrStr(bufferTemp, CPTEXT("file:///")) == bufferTemp) {
        // Strip off the entire host prefix
        prefixLength = CPStrLen(CPTEXT("file:///"));
    } else if (CPStrStr(bufferTemp, CPTEXT("file://localhost/")) == bufferTemp) {
        // Strip off the entire host prefix
        prefixLength = CPStrLen(CPTEXT("file://localhost/"));
    } else {
        // Strip off just file: (likely a UNC path)
        prefixLength = CPStrLen(CPTEXT("file:"));
    }
    CPCopyMemory(bufferTemp, CPCOUNT(bufferTemp), bufferTemp + prefixLength, CPCOUNT(bufferTemp) - prefixLength);

    CPURLNormalizeSlashes(bufferTemp, '\\');

    // NOTE: buffer must be at least MAX_PATH
    CPEXPECTTRUE(PathCanonicalize(buffer, bufferTemp));

    result = true;
CPCLEANUP:
    CPRelease(absoluteUrl);
    return result;
#endif
}

CP_API sal_out_opt CPURLRef CPPALConvertFileSystemPathToURL(sal_inout CPPALRef pal, sal_in_z const CPChar* buffer)
{
#if CP_PLATFORM(XBOX360)
    // TODO: XBOX360 CPPALConvertFileSystemPathToURL
    CPASSERTALWAYS();
    return NULL;
#else
    // Build URLs in the form of:
    // C:\My\Long Path.txt -> file:///C:/My/Long%20Path.txt

    // Setup the scratch buffer with 'file:///'
    CPChar scratchBuffer[8 + MAX_PATH];
    CPIGNORE(CPZeroMemory(scratchBuffer, sizeof(scratchBuffer), 0, sizeof(scratchBuffer)));
    CPStrCat(scratchBuffer, CPCOUNT(scratchBuffer), CPTEXT("file:///"));

    // Populate canonicalBuffer with the expanded path
    // NOTE: canonicalBuffer must be at least MAX_PATH
    CPChar* canonicalBuffer = scratchBuffer + 8;
    if (!PathCanonicalize(canonicalBuffer, buffer)) {
        return NULL;
    }

    // Flip all slashes around from \ to /
    CPChar* p = canonicalBuffer;
    while (*p) {
        if (*p == '\\') {
            *p = '/';
        }
        p++;
    }

    // scratchBuffer is now file:/// + canonicalBuffer and will contain something like 'file:///C:/My/Long Path.txt'

    // Escape scratchBuffer (we use a bigger buffer because we may be expanding by as much as 2x)
    CPChar encodedBuffer[2048];
    if (CPURLEscape(scratchBuffer, encodedBuffer, sizeof(encodedBuffer), CPURLEscapeOptionsPath) == -1) {
        return NULL;
    }

    return CPURLCreate(NULL, encodedBuffer);
#endif // XBOX360
}

#pragma mark -
#pragma mark Time

CP_API CPTime CPPALAbsoluteTimeGetCurrent(sal_inout CPPALRef pal)
{
    CPWin32PAL* win32pal = (CPWin32PAL*)pal;
    if (win32pal->useHighPerfTimer) {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        return counter.QuadPart * win32pal->invTicksPerSec;
    } else {
        // Using GetSystemTimeAsFileTime instead of GetSystemTime() as it has a 100ns resolution
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        ULARGE_INTEGER uli;
        uli.LowPart     = ft.dwLowDateTime;
        uli.HighPart    = ft.dwHighDateTime;
        return uli.QuadPart / 10000000.0;
    }
}

#pragma mark -
#pragma mark Debugging

// If not enabled, in shared CPPAL.c using stdout

#if CP_PAL_HAVE(LOGGING)

// TODO: use some fancy logging thing? System event log?
#error No Win32 logging implementation yet

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

CP_API sal_checkReturn sal_out_opt CPPALMutex* CPPALMutexCreate(sal_inout CPPALRef pal)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    // NOTE: because we use our own allocators here, we have to make sure they never try to create a mutex
    
    PCRITICAL_SECTION mutex = (PCRITICAL_SECTION)CPAlloc(sizeof(CRITICAL_SECTION));
    CPEXPECTNOTNULL(mutex);
    
    InitializeCriticalSection(mutex);
    
    return (CPPALMutex*)mutex;
    
CPCLEANUP:
    CPFree(mutex);
    return NULL;
}

CP_API void CPPALMutexDestroy(sal_inout CPPALMutex* mutex)
{
    PCRITICAL_SECTION p = (PCRITICAL_SECTION)mutex;
    DeleteCriticalSection(p);
    CPFree(mutex);
}

CP_API void CPPALMutexLock(sal_inout CPPALMutex* mutex)
{
    PCRITICAL_SECTION p = (PCRITICAL_SECTION)mutex;
    EnterCriticalSection(p);
}

CP_API void CPPALMutexUnlock(sal_inout CPPALMutex* mutex)
{
    PCRITICAL_SECTION p = (PCRITICAL_SECTION)mutex;
    LeaveCriticalSection(p);
}

#pragma mark -
#pragma mark Conditions

CP_API sal_checkReturn sal_out_opt CPPALCondition* CPPALConditionCreate(sal_inout CPPALRef pal)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    // NOTE: because we use our own allocators here, we have to make sure they never try to create a condition
    
    PCONDITION_VARIABLE cond = (PCONDITION_VARIABLE)CPAlloc(sizeof(CONDITION_VARIABLE));
    CPEXPECTNOTNULL(cond);

    InitializeConditionVariable(cond);
    
    return (CPPALCondition*)cond;
    
CPCLEANUP:
    CPFree(cond);
    return NULL;
}

CP_API void CPPALConditionDestroy(sal_inout CPPALCondition* condition)
{
    PCONDITION_VARIABLE cond = (PCONDITION_VARIABLE)condition;
    CPFree(cond);
}

CP_API BOOL CPPALConditionWait(sal_inout CPPALCondition* condition, sal_inout CPPALMutex* mutex)
{
    PCONDITION_VARIABLE cond = (PCONDITION_VARIABLE)condition;
    PCRITICAL_SECTION p = (PCRITICAL_SECTION)mutex;
    return (SleepConditionVariableCS(cond, p, INFINITE) != 0);
}

CP_API void CPPALConditionBroadcast(sal_inout CPPALCondition* condition)
{
    PCONDITION_VARIABLE cond = (PCONDITION_VARIABLE)condition;
    WakeAllConditionVariable(cond);
}

#pragma mark -
#pragma mark Heaps

// In shared CPPAL.c using system allocators

#pragma mark -
#pragma mark Thread Management

void CPPALThreadSetPriorityCore(HANDLE thread, const CPPALThreadPriority priority)
{
    switch (priority) {
        case CPPALThreadPriorityIdle:
            SetThreadPriority(thread, THREAD_PRIORITY_IDLE);
            break;
        case CPPALThreadPriorityLow:
            SetThreadPriority(thread, THREAD_PRIORITY_BELOW_NORMAL);
            break;
        default:
        case CPPALThreadPriorityNormal:
            SetThreadPriority(thread, THREAD_PRIORITY_NORMAL);
            break;
        case CPPALThreadPriorityHigh:
            SetThreadPriority(thread, THREAD_PRIORITY_ABOVE_NORMAL);
            break;
        case CPPALThreadPriorityRealtime:
            SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST);
            break;
    }
}

typedef struct CPWin32ThreadHandle_t {
    CPPALRef            pal;
    HANDLE              thread;
    uint32            options;
    CPPALThreadEntry    entry;
    void*               state;      // Cleared immediately after thread starts
    volatile int        alive;
} CPWin32ThreadHandle;

sal_checkReturn sal_out_opt CPWin32ThreadHandle* CPWin32ThreadHandleCreate(sal_inout CPPALRef pal)
{
    CPWin32ThreadHandle* handle = (CPWin32ThreadHandle*)CPAlloc(sizeof(CPWin32ThreadHandle));
    CPEXPECTNOTNULL(handle);
    
    handle->pal     = CPPALRetain(pal);
    
    handle->alive   = 1;
    
    return handle;
    
CPCLEANUP:
    if (handle) {
        CPRelease(handle->pal);
        CPFree(handle);
    }
    return NULL;
}

void CPWin32ThreadHandleDestroy(sal_inout CPWin32ThreadHandle* handle)
{
    if (handle->thread) {
        CPIGNORE(CloseHandle(handle->thread));
        handle->thread = NULL;
    }
    CPRelease(handle->pal);
    CPFree(handle);
}

sal_callback DWORD __stdcall CPWin32ThreadEntryThunk(sal_inout CPWin32ThreadHandle* handle)
{
    // Grab a pointer to the state since the entry point will likely free it
    void* state = handle->state;
    handle->state = NULL;

#if !CP_PLATFORM(XBOX360)
    CPIGNORE(CoInitializeEx(NULL, COINIT_MULTITHREADED));
#endif // XBOX360
    
    void* block = CPPALThreadBlockBegin(handle->pal);
    
    // Call the entry point
    handle->entry(handle->pal, state);
    state = NULL;
    
    CPPALThreadBlockEnd(handle->pal, block);
    
    CPAtomicDec32(&handle->alive);

#if !CP_PLATFORM(XBOX360)
    CoUninitialize();
#endif // XBOX360

    ExitThread(0);
    return 0;
}

CP_API sal_checkReturn sal_out_opt CPPALThread* CPPALThreadCreate(sal_inout CPPALRef pal, const uint32 options, CPPALThreadEntry entry, sal_inout_opt void* state)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    CPWin32ThreadHandle* handle = CPWin32ThreadHandleCreate(pal);
    CPEXPECTNOTNULL(handle);
    
    handle->options     = options;
    handle->entry       = entry;
    handle->state       = state;
    
    // Setup thread attributes
    DWORD stackSize = 0;
    DWORD flags     = 0;
    if (options & CPPALThreadOptionsJoinable) {
        // All threads are joinable
    }
    if (options & CPPALThreadOptionsSmallStack) {
        stackSize = 64 * 1024; // TODO: too small?
    }
    
    // Create thread
    HANDLE thread = CreateThread(
        NULL,
        stackSize,
        (LPTHREAD_START_ROUTINE)CPWin32ThreadEntryThunk,
        handle,
        flags,
        NULL
    );
    CPEXPECTNOTNULL(thread);

    handle->thread = thread;
    
    return (CPPALThread*)handle;
    
CPCLEANUP:
    if (handle) {
        CPWin32ThreadHandleDestroy(handle);
    }
    return NULL;
}

CP_API BOOL CPPALThreadJoin(sal_inout CPPALRef pal, sal_inout CPPALThread* thread)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    CPWin32ThreadHandle* handle = (CPWin32ThreadHandle*)thread;
    
    // Thread must be joinable
    CPASSERTTRUE(handle->options & CPPALThreadOptionsJoinable);
    if (!(handle->options & CPPALThreadOptionsJoinable)) {
        return FALSE;
    }

    const BOOL joined = (WaitForSingleObject(handle->thread, INFINITE) != WAIT_TIMEOUT);
    if (!joined) {
        // Failed to join
        return FALSE;
    }
    
    // Succeeded or thread already dead
    CPWin32ThreadHandleDestroy(handle);
    return TRUE;
}

CP_API void CPPALThreadShutdown(sal_inout CPPALRef pal, sal_inout CPPALThread* thread, sal_inout CPPALCondition* condition)
{
    CPPALThreadShutdownList(pal, &thread, 1, condition);
}

CP_API void CPPALThreadShutdownList(sal_inout CPPALRef pal, sal_inout_ecount(count) CPPALThread** threads, const uint32 count, sal_inout CPPALCondition* condition)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    // Count the number of currently living threads
    uint32 liveThreads = 0;
    for (uint32 n = 0; n < count; n++) {
        CPWin32ThreadHandle* handle = (CPWin32ThreadHandle*)threads[n];
        if (handle) {
            liveThreads++;

            // Raise priority so that it has the cycles to shut down
            CPPALThreadSetPriorityCore(handle->thread, CPPALThreadPriorityHigh);
        }
    }
    
    while (liveThreads > 0) {
        for (uint32 n = 0; n < count; n++) {
            CPWin32ThreadHandle* handle = (CPWin32ThreadHandle*)threads[n];
            if (!handle) {
                continue;
            }
            
            BOOL hasDied = FALSE;
            if (handle->alive) {
                // Poke the thread
                CPPALConditionBroadcast(condition);
                
                // Wait on it for 10ms
                hasDied = (WaitForSingleObject(handle->thread, 10) != WAIT_TIMEOUT);
            } else {
                // Thread already died
                hasDied = TRUE;
            }
            
            if (hasDied) {
                // Cleanup thread
                CPWin32ThreadHandleDestroy(handle);
                threads[n] = NULL;
                liveThreads--;
            }
        }
    }
}

#pragma mark -
#pragma mark Thread Operations

CP_API BOOL CPPALThreadIsMain(sal_inout CPPALRef pal)
{
    CPWin32PAL* win32pal = (CPWin32PAL*)pal;
    return (GetCurrentThreadId() == win32pal->mainThreadId);
}

CP_API sal_out void* CPPALThreadGetID(sal_inout CPPALRef pal)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    return (void*)GetCurrentThreadId();
}

// This magnificently ugly code is from:
// http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
#pragma warning(disable : 6320; disable : 6322)
CP_API void CPPALThreadSetName(sal_inout CPPALRef pal, sal_in_z const CPChar* name)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);

    if (!IsDebuggerPresent()) {
        return;
    }

#if defined(CP_WCHAR)
    char threadName[256];
    if (wcstombs_s(NULL, threadName, sizeof(threadName), name, sizeof(threadName))) {
        return;
    }
#else
    const char* threadName = name;
#endif

    THREADNAME_INFO info;
    info.dwType     = 0x1000;
    info.szName     = threadName;
    info.dwThreadID = (DWORD)-1;
    info.dwFlags    = 0;
    __try {
        RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
    }
}
#pragma warning(default : 6320; default : 6322)

CP_API void CPPALThreadSetPriority(sal_inout CPPALRef pal, const CPPALThreadPriority priority)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    HANDLE thread = GetCurrentThread();
    if (!thread) {
        return;
    }
    CPPALThreadSetPriorityCore(thread, priority);
    CPIGNORE(CloseHandle(thread));
}

CP_API void CPPALThreadSleep(sal_inout CPPALRef pal, const CPTime delay)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    const DWORD ms = (DWORD)(delay * 1000.0);
    CPIGNORE(SleepEx(ms, TRUE));
}

#pragma mark -
#pragma mark Thread Blocks

// In shared CPPAL.c using default implementation

#pragma mark -
#pragma mark File Mapping

CP_API sal_out_opt CPPALFileMapping* CPPALFileMappingCreate(sal_inout CPPALRef pal, sal_inout CPURLRef path, const CPPALFileMappingMode mode, const size_t offset, const size_t length)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);

    CPPALFileMapping* mapping = (CPPALFileMapping*)CPAlloc(sizeof(CPPALFileMapping));
    if (!mapping) {
        return NULL;
    }

    DWORD fileAccess        = 0;
    DWORD fileShare         = 0;
    DWORD createMode        = 0;
    DWORD mappingProtect    = 0;
    DWORD viewAccess        = 0;
    switch (mode) {
    case CPPALFileMappingModeRead:
        fileAccess      |= GENERIC_READ;
        fileShare       |= FILE_SHARE_READ;
        createMode      |= OPEN_EXISTING;
        mappingProtect  |= PAGE_READONLY;
        viewAccess      |= FILE_MAP_READ;
        break;
    case CPPALFileMappingModeWrite:
        fileAccess      |= GENERIC_WRITE;
        fileShare       |= 0;
        createMode      |= OPEN_ALWAYS;
        mappingProtect  |= PAGE_READWRITE;
        viewAccess      |= FILE_MAP_WRITE;
        break;
    case CPPALFileMappingModeReadWrite:
        fileAccess      |= GENERIC_READ | GENERIC_WRITE;
        fileShare       |= 0;
        createMode      |= OPEN_EXISTING;
        mappingProtect  |= PAGE_READWRITE;
        viewAccess      |= FILE_MAP_READ | FILE_MAP_WRITE;
        break;
    }

    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);

    CPChar fullPath[CP_MAX_PATH];
    CPEXPECTTRUE(CPPALConvertURLToFileSystemPath(pal, path, fullPath, CPCOUNT(fullPath)));

    HANDLE file = CreateFile(fullPath, fileAccess, fileShare, NULL, createMode, FILE_ATTRIBUTE_NORMAL, NULL);
    CPEXPECTNOTNULL(file);

    size_t alignedOffset = offset & (~(systemInfo.dwAllocationGranularity - 1));
    size_t alignedLength = length + (offset - alignedOffset);

    HANDLE handle = CreateFileMapping(file, NULL, mappingProtect, 0, 0, NULL);//(DWORD)(alignedLength >> 32), (DWORD)(alignedLength & 0xFFFFFFFF), NULL);
    CPEXPECTNOTNULL(handle);

    void* address = MapViewOfFile(handle, viewAccess, (DWORD)(alignedOffset >> 32), (DWORD)(alignedOffset & 0xFFFFFFFF), alignedLength);
    DWORD dd = GetLastError();
    CPEXPECTNOTNULL(address);

    mapping->file       = file;
    mapping->handle     = handle;
    mapping->rawAddress = address;
    mapping->address    = (byte*)address + (offset - alignedOffset);
    mapping->length     = length;

    return mapping;

CPCLEANUP:
    if (address) {
        UnmapViewOfFile(address);
    }
    if (handle) {
        CloseHandle(handle);
    }
    if (file) {
        CloseHandle(file);
    }
    return NULL;
}

CP_API void CPPALFileMappingFlush(sal_inout CPPALRef pal, sal_inout CPPALFileMapping* mapping)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);

    FlushViewOfFile(mapping->rawAddress, 0);
}

CP_API void CPPALFileMappingDestroy(sal_inout CPPALRef pal, sal_inout CPPALFileMapping* mapping)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);

    UnmapViewOfFile(mapping->rawAddress);
    CloseHandle(mapping->handle);
    CloseHandle(mapping->file);
    CPFree(mapping);
}
