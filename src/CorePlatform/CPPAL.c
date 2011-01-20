/*
 *  CPPAL.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPPAL.h>
#include <CorePlatform/CPCommon.h>

#include <time.h>

#if !CP_PAL_HAVE(THREADS)
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/time.h>
#endif

CP_DEFINE_TYPE(CPPAL, NULL, CPPALDealloc);

CP_API void CPPALOptionsInitialize(sal_inout CPPALOptions* options)
{
    CPZeroMemory(options, sizeof(CPPALOptions), 0, sizeof(CPPALOptions));
}

CP_API sal_checkReturn sal_out_bcount_opt(size) CPPALRef CPPALAlloc(sal_in const CPObjectType* type, const size_t size, const CPPALOptions* options)
{
    CPASSERT(size >= sizeof(CPPAL));
    CPPALRef pal = (CPPALRef)CPObjectAlloc(type, size);
    if (!pal) {
        return NULL;
    }

    // Copy options local
    CPPALOptions* destOptions = (CPPALOptions*)&pal->options;
    CPEXPECTTRUE(CPCopyMemory(destOptions, sizeof(CPPALOptions), options, sizeof(CPPALOptions)));

    // Assign default options
    if (CPStrLen(destOptions->applicationName) == 0) {
        CPIGNORE(CPStrCpy(destOptions->applicationName, CPCOUNT(destOptions->applicationName), CPTEXT("CorePlatform")));
    }

    srand((unsigned int)time(NULL));
    
    return pal;
    
CPCLEANUP:
    CPRelease(pal);
    return NULL;
}

void CPPALDealloc(sal_inout CPPALRef pal)
{
    for (size_t n = 0; n < CPCOUNT(pal->systemPaths); n++) {
        CPRelease(pal->systemPaths[n]);
    }
}

#pragma mark -
#pragma mark System Info

CP_API sal_out_opt CPURLRef CPPALSystemGetPath(sal_inout CPPALRef pal, const CPPALSystemPath systemPath)
{
    if (systemPath >= CPCOUNT(pal->systemPaths)) {
        return NULL;
    }
    return pal->systemPaths[systemPath];
}

#pragma mark -
#pragma mark Path Utilities

#if !CP_PAL_HAVE(PATHUTILS)

CP_API BOOL CPPALConvertURLToFileSystemPath(sal_inout CPPALRef pal, sal_inout CPURLRef url, sal_out_bcount(bufferSize) CPChar* buffer, const size_t bufferSize)
{
    CPASSERTALWAYS();
    return FALSE;
}

CP_API sal_out_opt CPURLRef CPPALConvertFileSystemPathToURL(sal_inout CPPALRef pal, sal_in_z const CPChar* buffer)
{
    // Incoming is expected to be /path/file.etc
    if (buffer[0] != '/') {
        return NULL;
    }
    
    CPURLRef baseURL = NULL;
    CPURLRef finalURL = NULL;
    
    baseURL = CPURLCreate(NULL, CPTEXT("file://localhost/"));
    CPEXPECTNOTNULL(baseURL);
    finalURL = CPURLCreate(baseURL, buffer);
    CPEXPECTNOTNULL(finalURL);
    CPRelease(baseURL);
    return finalURL;
    
CPCLEANUP:
    CPRelease(finalURL);
    CPRelease(baseURL);
    return NULL;
}

#endif // !PATHUTILS

#pragma mark -
#pragma mark Random Numbers

#if !CP_PAL_HAVE(RANDOM)

CP_API int32 CPPALRandomInt32(sal_inout CPPALRef pal)
{
    return rand();
}

CP_API double CPPALRandomDouble(sal_inout CPPALRef pal)
{
    return (rand() / (double)RAND_MAX);
}

#endif // RANDOM

#pragma mark -
#pragma mark Debugging

// NOTE: this path separator check may not be enough - cygwin may mess it up/etc
#if CP_LIKE(WIN32)
#define CP_PATH_SEPARATOR   ((CPChar)'\\')
#else
#define CP_PATH_SEPARATOR   ((CPChar)'/')
#endif

#define CP_LOG_MAX          2048

#if !CP_PAL_HAVE(LOGGING)

CP_API void CPPALLogMessage(sal_in_z const CPChar* filePath, const int32 lineNumber, sal_in_z const CPChar* functionName, sal_in_z_opt const CPChar* tag, const CPPALLogLevel level, sal_in_z const CPChar* fmt, ...)
{
    // Strip out just the filename from the path
    const CPChar* filename = CPStrRChr(filePath, CP_PATH_SEPARATOR);
    if (filename) {
        // Slash - skip over it
        filename++;
    } else {
        // No slash, entire thing is filename
        filename = filePath;
    }
    
    CPChar levelChar;
    switch (level) {
        case CPPALLogLevelError:
            levelChar = '!';
            break;
        case CPPALLogLevelWarning:
            levelChar = 'w';
            break;
        case CPPALLogLevelInfo:
            levelChar = 'i';
            break;
        default:
        case CPPALLogLevelDebug:
            levelChar = 'd';
            break;
    }
    
    va_list args;
    va_start(args, fmt);
    CPChar buffer[CP_LOG_MAX];
    int bufferLength = CPVSNPrintF(buffer, CPCOUNT(buffer), fmt, args);
    va_end(args);
    if (bufferLength < 0) {
        return;
    }
    
    // Format string - add a trailing newline if required
    const CPChar* outfmt;
    if ((bufferLength >= 1) && buffer[bufferLength - 1] == '\n') {
        outfmt = CPTEXT("CP[%c] %s:%d: %s");
    } else {
        outfmt = CPTEXT("CP[%c] %s:%d: %s\n");
    }
    
#if defined(OutputDebugString)
    CPChar fullOutput[CP_LOG_MAX];
    if (CPSNPrintF(fullOutput, CPCOUNT(buffer), outfmt, levelChar, filename, lineNumber, buffer) >= 0) {
        OutputDebugString(fullOutput);
    }
#elif defined(CP_WCHAR)
    CPIGNORE(fwprintf(stdout, outfmt, levelChar, filename, lineNumber, buffer));
#else
    CPIGNORE(fprintf(stdout, outfmt, levelChar, filename, lineNumber, buffer));
#endif
}

CP_API void CPPALLogData(sal_in_z const CPChar* filePath, const int32 lineNumber, sal_in_z const CPChar* functionName, sal_in_z_opt const CPChar* tag, const CPPALLogLevel level, sal_in_bcount(size) const byte* data, const size_t size)
{
    CPPALLogMessage(filePath, lineNumber, functionName, tag, level, CPTEXT("attempting to log %d bytes of data"), (int32)size);
}

CP_API void CPPALLogBitmap(sal_in_z const CPChar* filePath, const int32 lineNumber, sal_in_z const CPChar* functionName, sal_in_z_opt const CPChar* tag, const CPPALLogLevel level, const uint32 width, const uint32 height, const uint32 bytesPerPixel, sal_in_bcount(size) const byte* bitmap, const size_t size)
{
    CPPALLogMessage(filePath, lineNumber, functionName, tag, level, CPTEXT("attempting to log bitmap %dpx x %dpx x %dbpp"), width, height, bytesPerPixel * 8);
}

#endif // LOGGING

#pragma mark -
#pragma mark Mutexes

#if !CP_PAL_HAVE(THREADS)

CP_API sal_checkReturn sal_out_opt CPPALMutex* CPPALMutexCreate(sal_inout CPPALRef pal)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    // NOTE: because we use our own allocators here, we have to make sure they never try to create a mutex
    
    pthread_mutex_t* mutex = (pthread_mutex_t*)CPAlloc(sizeof(pthread_mutex_t));
    CPEXPECTNOTNULL(mutex);
    
    CPEXPECTZERO(pthread_mutex_init(mutex, NULL));
    
    return (CPPALMutex*)mutex;
    
CPCLEANUP:
    CPFree(mutex);
    return NULL;
}

CP_API void CPPALMutexDestroy(sal_inout CPPALMutex* mutex)
{
    pthread_mutex_t* p = (pthread_mutex_t*)mutex;
    CPIGNORE(pthread_mutex_destroy(p));
    CPFree(mutex);
}

CP_API void CPPALMutexLock(sal_inout CPPALMutex* mutex)
{
    pthread_mutex_t* p = (pthread_mutex_t*)mutex;
    CPIGNORE(pthread_mutex_lock(p));
}

CP_API void CPPALMutexUnlock(sal_inout CPPALMutex* mutex)
{
    pthread_mutex_t* p = (pthread_mutex_t*)mutex;
    CPIGNORE(pthread_mutex_unlock(p));
}

#endif // !THREADS

#pragma mark -
#pragma mark Conditions

#if !CP_PAL_HAVE(THREADS)

CP_API sal_checkReturn sal_out_opt CPPALCondition* CPPALConditionCreate(sal_inout CPPALRef pal)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    // NOTE: because we use our own allocators here, we have to make sure they never try to create a condition
    
    pthread_cond_t* cond = (pthread_cond_t*)CPAlloc(sizeof(pthread_cond_t));
    CPEXPECTNOTNULL(cond);
    
    CPEXPECTZERO(pthread_cond_init(cond, NULL));
    
    return (CPPALCondition*)cond;
    
CPCLEANUP:
    CPFree(cond);
    return NULL;
}

CP_API void CPPALConditionDestroy(sal_inout CPPALCondition* condition)
{
    pthread_cond_t* cond = (pthread_cond_t*)condition;
    while (pthread_cond_destroy(cond) == EBUSY) {
        CPIGNORE(pthread_cond_broadcast(cond));
        // NOTE: pthread_yield_np is not supported everywhere, but doesn't have a good define to check
        // NOTE: if pthread_yield_np is not supported, try sleeping instead
        CPIGNORE(pthread_yield_np());
    }
    
    CPFree(cond);
}

CP_API BOOL CPPALConditionWait(sal_inout CPPALCondition* condition, sal_inout CPPALMutex* mutex)
{
    pthread_cond_t* cond = (pthread_cond_t*)condition;
    pthread_mutex_t* p = (pthread_mutex_t*)mutex;
    return (pthread_cond_wait(cond, p) == 0);
}

CP_API void CPPALConditionBroadcast(sal_inout CPPALCondition* condition)
{
    pthread_cond_t* cond = (pthread_cond_t*)condition;
    CPIGNORE(pthread_cond_broadcast(cond));
}

#endif // !THREADS

#pragma mark -
#pragma mark Heaps

#if !CP_PAL_HAVE(HEAPS)

// This is a fallback implementation of heaps that uses the standard allocators
// It is recommended that platform-specific heap functionality is used whenever available
// NOTE: even though we are using the system heap, we still keep track of space used/etc
// to make debugging easier and for supporting fixed-size heaps

typedef struct CPPALSystemHeap_t {
    uint32          options;
    size_t          initialSize;

    CPPALMutex*     mutex;
    size_t          allocated;
} CPPALSystemHeap;

CP_API sal_checkReturn sal_out_opt CPPALHeap* CPPALHeapCreate(sal_inout CPPALRef pal, const uint32 options, const size_t initialSize)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    CPPALSystemHeap* heap = (CPPALSystemHeap*)CPAlloc(sizeof(CPPALSystemHeap));
    CPEXPECTNOTNULL(heap);
    
    heap->options       = options;
    heap->initialSize   = initialSize;

    heap->mutex         = CPPALMutexCreate(pal);
    CPEXPECTNOTNULL(heap->mutex);
    heap->allocated     = 0;
    
    return heap;

CPCLEANUP:
    if (heap) {
        CPFree(heap);
    }
    return NULL;
}

CP_API void CPPALHeapDestroy(sal_inout CPPALHeap* heap)
{
    CPPALSystemHeap* p = (CPPALSystemHeap*)heap;
    if (p->mutex) {
        CPPALMutexDestroy(p->mutex);
    }
    CPASSERT(p->allocated == 0);
    CPFree(heap);
}

CP_API sal_checkReturn sal_out_bcount_opt(size) void* CPPALHeapAlloc(sal_inout CPPALHeap* heap, const size_t size)
{
    CPPALSystemHeap* p = (CPPALSystemHeap*)heap;
    
    if (p->options & CPPALHeapOptionsFixedSize) {
        CPPALMutexLock(p->mutex);
        const BOOL overflowing = (p->allocated + size > p->initialSize);
        CPPALMutexUnlock(p->mutex);
        if (overflowing) {
            return NULL;
        }
    }
    
    void* ptr = CPAlloc(sizeof(size_t) + size);
    if (!ptr) {
        return NULL;
    }
    
    CPPALMutexLock(p->mutex);
    p->allocated += size;
    CPPALMutexUnlock(p->mutex);
    
    size_t* sp = (size_t*)ptr;
    *sp = size;
    sp++;
    
    return (void*)sp;
}

CP_API sal_checkReturn sal_out_bcount_opt(newSize) void* CPPALHeapRealloc(sal_inout CPPALHeap* heap, sal_inout_bcount_opt(oldSize) void* ptr, const size_t oldSize, const size_t newSize)
{
    CPPALSystemHeap* p = (CPPALSystemHeap*)heap;
    
    if (p->options & CPPALHeapOptionsFixedSize) {
        CPPALMutexLock(p->mutex);
        const BOOL overflowing = (((p->allocated - oldSize) + newSize) > p->initialSize);
        CPPALMutexUnlock(p->mutex);
        if (overflowing) {
            return NULL;
        }
    }
    
    size_t* spo = (size_t*)ptr;
    if (spo) {
        spo--;
        CPASSERT(*spo == oldSize);
    }
    
    void* result = CPRealloc(spo, sizeof(size_t) + oldSize, sizeof(size_t) + newSize);
    if (!result) {
        return NULL;
    }

    CPPALMutexLock(p->mutex);
    p->allocated = p->allocated + newSize - oldSize;
    CPPALMutexUnlock(p->mutex);
    
    size_t* spn = (size_t*)result;
    *spn = newSize;
    spn++;

    return (void*)spn;
}

CP_API void CPPALHeapFree(sal_inout CPPALHeap* heap, sal_inout void* ptr)
{
    CPPALSystemHeap* p = (CPPALSystemHeap*)heap;
    
    size_t* sp = (size_t*)ptr;
    sp--;
    size_t originalSize = *sp;
    
    CPPALMutexLock(p->mutex);
    p->allocated -= originalSize;
    CPPALMutexUnlock(p->mutex);
    
    CPFree(sp);
}

#endif // !HEAPS

#pragma mark -
#pragma mark Thread Management

#if !CP_PAL_HAVE(THREADS)

#ifndef PTHREAD_MAX_PRIORITY
#define PTHREAD_MAX_PRIORITY 31
#endif
#ifndef PTHREAD_MIN_PRIORITY
#define PTHREAD_MIN_PRIORITY 0
#endif

#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
void CPPALThreadSetPriorityCore(pthread_t thread, const CPPALThreadPriority priority)
{
    int policy;
    struct sched_param param;
    CPEXPECTZERO(pthread_getschedparam(thread, &policy, &param));
    
#if defined(_POSIX_PRIORITY_SCHEDULING)
    const int minPriority = sched_get_priority_min(SCHED_OTHER);
    const int maxPriority = sched_get_priority_max(SCHED_OTHER);
#else
    const int minPriority = PTHREAD_MIN_PRIORITY;
    const int maxPriority = PTHREAD_MAX_PRIORITY;
#endif
    
    int newPriority = param.sched_priority;
    switch (priority) {
        case CPPALThreadPriorityIdle:
            newPriority = (maxPriority - minPriority) * 0.00f + minPriority;
            break;
        case CPPALThreadPriorityLow:
            newPriority = (maxPriority - minPriority) * 0.25f + minPriority;
            break;
        default:
        case CPPALThreadPriorityNormal:
            newPriority = (maxPriority - minPriority) * 0.50f + minPriority;
            break;
        case CPPALThreadPriorityHigh:
            newPriority = (maxPriority - minPriority) * 0.75f + minPriority;
            break;
        case CPPALThreadPriorityRealtime:
            newPriority = (maxPriority - minPriority) * 1.00f + minPriority;
            break;
    }
    param.sched_priority = newPriority;
    
    CPEXPECTZERO(pthread_setschedparam(thread, policy, &param));
    return;
CPCLEANUP:
    return;
}
#else
void CPPALThreadSetPriorityCore(pthread_t thread, const CPPALThreadPriority priority)
{
    CPUNREFERENCED(thread);
    CPUNREFERENCED(priority);
    #warning No pthreads priority support - all threads will run at CPPALThreadPriorityNormal
}
#endif // _POSIX_THREAD_PRIORITY_SCHEDULING

typedef struct CPPThreadHandle_t {
    CPPALRef            pal;
    pthread_t           thread;
    uint32              options;
    CPPALThreadEntry    entry;
    void*               state;      // Cleared immediately after thread starts
    pthread_mutex_t     mutex;
    pthread_cond_t      cond;
    volatile int        alive;
} CPPThreadHandle;

sal_checkReturn sal_out_opt CPPThreadHandle* CPPThreadHandleCreate(sal_inout CPPALRef pal)
{
    CPPThreadHandle* handle = (CPPThreadHandle*)CPAlloc(sizeof(CPPThreadHandle));
    CPEXPECTNOTNULL(handle);
    
    handle->pal     = CPPALRetain(pal);
    
    CPEXPECTZERO(pthread_mutex_init(&handle->mutex, NULL));
    CPEXPECTZERO(pthread_cond_init(&handle->cond, NULL));
    
    handle->alive   = 1;
    
    return handle;
    
CPCLEANUP:
    CPRelease(handle->pal);
    CPFree(handle);
    return NULL;
}

void CPPThreadHandleDestroy(sal_inout CPPThreadHandle* handle)
{
    pthread_cond_destroy(&handle->cond);
    pthread_mutex_destroy(&handle->mutex);
    handle->thread = NULL;
    CPRelease(handle->pal);
    CPFree(handle);
}

sal_callback void* CPPThreadEntryThunk(sal_inout CPPThreadHandle* handle)
{
    // Grab a pointer to the state since the entry point will likely free it
    void* state = handle->state;
    handle->state = NULL;
    
    void* block = CPPALThreadBlockBegin(handle->pal);
    
    // Call the entry point
    handle->entry(handle->pal, state);
    state = NULL;
    
    CPPALThreadBlockEnd(handle->pal, block);
    
    // If joinable, signal any waiters trying to join
    if (handle->options & CPPALThreadOptionsJoinable) {
        CPAtomicDec32(&handle->alive);
        pthread_mutex_lock(&handle->mutex);
        pthread_cond_signal(&handle->cond);
        pthread_mutex_unlock(&handle->mutex);
    }
    
    CPAtomicDec32(&handle->alive);
    
    CPIGNORE(pthread_exit(NULL));
    return NULL;
}

CP_API sal_checkReturn sal_out_opt CPPALThread* CPPALThreadCreate(sal_inout CPPALRef pal, const uint32 options, CPPALThreadEntry entry, sal_inout_opt void* state)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    CPPThreadHandle* handle = CPPThreadHandleCreate(pal);
    CPEXPECTNOTNULL(handle);
    
    handle->options     = options;
    handle->entry       = entry;
    handle->state       = state;
    
    // Setup thread attributes
    pthread_attr_t attr;
    CPIGNORE(pthread_attr_init(&attr));
    if (options & CPPALThreadOptionsJoinable) {
        CPIGNORE(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
    } else {
        CPIGNORE(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
    }
    if (options & CPPALThreadOptionsSmallStack) {
        CPIGNORE(pthread_attr_setstacksize(&attr, 64 * 1024)); // TODO: too small?
    }
    
    // Create thread
    const int result = pthread_create(&handle->thread, &attr, (void *(*)(void *))CPPThreadEntryThunk, handle);
    
    // Cleanup attributes
    CPIGNORE(pthread_attr_destroy(&attr));
    
    // Handle failure
    CPEXPECTZERO(result);
    
    return (CPPALThread*)handle;
    
CPCLEANUP:
    if (handle) {
        CPPThreadHandleDestroy(handle);
    }
    return NULL;
}

CP_API BOOL CPPALThreadJoin(sal_inout CPPALRef pal, sal_inout CPPALThread* thread)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    CPPThreadHandle* handle = (CPPThreadHandle*)thread;
    
    // Thread must be joinable
    CPASSERTTRUE(handle->options & CPPALThreadOptionsJoinable);
    if (!(handle->options & CPPALThreadOptionsJoinable)) {
        return FALSE;
    }
    
    int result = pthread_join(handle->thread, NULL);
    if ((result == 0) || (result == ESRCH)) {
        // Succeeded or thread already dead
        CPPThreadHandleDestroy(handle);
        return TRUE;
    } else {
        // Failed to join
        return FALSE;
    }
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
        CPPThreadHandle* handle = (CPPThreadHandle*)threads[n];
        if (handle) {
            liveThreads++;

            // Raise priority so that it has the cycles to shut down
            CPPALThreadSetPriorityCore(handle->thread, CPPALThreadPriorityHigh);
        }
    }
    
    while (liveThreads > 0) {
        for (uint32 n = 0; n < count; n++) {
            CPPThreadHandle* handle = (CPPThreadHandle*)threads[n];
            if (!handle) {
                continue;
            }
            
            BOOL hasDied = FALSE;
            if (handle->alive) {
                // Poke the thread
                CPPALConditionBroadcast(condition);
                
                // Wait on it for 10ms
                pthread_mutex_lock(&handle->mutex);
                struct timeval tv;
                gettimeofday(&tv, NULL);
                struct timespec ts;
                ts.tv_sec   = tv.tv_sec;
                ts.tv_nsec  = tv.tv_usec * 1000;
                ts.tv_nsec  += 10 * 1000000;
                hasDied = (pthread_cond_timedwait(&handle->cond, &handle->mutex, &ts) == 0);
                pthread_mutex_unlock(&handle->mutex);
            } else {
                // Thread already died
                hasDied = TRUE;
            }
            
            if (hasDied) {
                // Cleanup thread
                CPPThreadHandleDestroy(handle);
                threads[n] = NULL;
                liveThreads--;
            }
        }
    }
}

#endif // !THREADS

#pragma mark -
#pragma mark Thread Operations

#if !CP_PAL_HAVE(THREADS)

CP_API BOOL CPPALThreadIsMain(sal_inout CPPALRef pal)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    return (pthread_main_np() != 0);
}

CP_API sal_out void* CPPALThreadGetID(sal_inout CPPALRef pal)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    return (void*)pthread_self();
}

CP_API void CPPALThreadSetName(sal_inout CPPALRef pal, sal_in_z const CPChar* name)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    CPIGNORE(pthread_setname_np(name));
}

CP_API void CPPALThreadSetPriority(sal_inout CPPALRef pal, const CPPALThreadPriority priority)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    pthread_t thread = pthread_self();
    CPPALThreadSetPriorityCore(thread, priority);
}

CP_API void CPPALThreadSleep(sal_inout CPPALRef pal, const CPTime delay)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    
    // http://cc.byexamples.com/2007/05/25/nanosleep-is-better-than-sleep-and-usleep/
    const time_t sec    = ( time_t )delay;
    const time_t msec   = ( time_t )(delay - sec);
    struct timespec req;
    req.tv_sec          = sec;
    req.tv_nsec         = msec * 1000000L;
    while (nanosleep(&req, &req) == -1) {
        if (errno != EINTR) {
            // Failed
            break;
        }
        continue;
    }
}

#endif // !THREADS

#pragma mark -
#pragma mark Thread Blocks

#if !CP_PAL_HAVE(THREADBLOCKS)

#define CP_DUMMY_THREAD_BLOCK ((void*)0xDEADBEEF)

CP_API sal_out void* CPPALThreadBlockBegin(sal_inout CPPALRef pal)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    return CP_DUMMY_THREAD_BLOCK;
}

CP_API void CPPALThreadBlockEnd(sal_inout CPPALRef pal, sal_inout void* block)
{
    CPUNREFERENCED(pal);
    CPASSERTNOTNULL(pal);
    CPASSERT(block == CP_DUMMY_THREAD_BLOCK);
}

#endif // !THREADBLOCKS
