/*
 *  test_pal.c
 *  CPUnitTests-OSX
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "shared.h"

static int test_pal_init()
{
    return 0;
}

static int test_pal_clean()
{
    return 0;
}

void test_pal_creation()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);
    CU_ASSERT(pal != NULL);
    CPRelease(pal);
}

void test_pal_systeminfo()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CPPALSystemInfo sysInfo;
    CU_ASSERT_TRUE(CPPALSystemInfoQuery(pal, &sysInfo));
    CU_ASSERT(sysInfo.processors.physicalCount >= 1);
    CU_ASSERT(sysInfo.processors.logicalCount >= 1);
    CU_ASSERT(sysInfo.processors.logicalCount >= sysInfo.processors.physicalCount);

    CPRelease(pal);
}

void test_pal_systempaths()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    // TODO: test system paths
    // CPPALSystemGetPath

    CPRelease(pal);
}

void test_pal_pathconversion()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    // TODO: test path conversion
    //CPPALConvertURLToFileSystemPath
    //CPPALConvertFileSystemPathToURL

    CPRelease(pal);
}

void test_pal_time()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CPTime t0 = CPPALAbsoluteTimeGetCurrent(pal);
    CU_ASSERT(t0 > 0.0);

    CPPALThreadSleep(pal, 1.0);

    CPTime t1 = CPPALAbsoluteTimeGetCurrent(pal);
    CU_ASSERT(t1 >= t0);
    CU_ASSERT(t1 - t0 < 1.5);

    CPRelease(pal);
}

void test_pal_random()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    BOOL anyFailed = FALSE;

    anyFailed = FALSE;
    for (size_t n = 0; n < 1000; n++) {
        const int32 r = CPPALRandomInt32(pal);
        if (r < 0) {
            anyFailed = TRUE;
            break;
        }
    }
    CU_ASSERT_FALSE(anyFailed);

    anyFailed = FALSE;
    for (size_t n = 0; n < 1000; n++) {
        const double r = CPPALRandomDouble(pal);
        if ((r < 0.0) || (r > 1.0)) {
            anyFailed = TRUE;
            break;
        }
    }
    CU_ASSERT_FALSE(anyFailed);

    CPRelease(pal);
}

void test_pal_mutex()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CPPALMutex* m;

    m = CPPALMutexCreate(pal);
    CU_ASSERT(m != NULL);
    CPPALMutexDestroy(m);
    m = NULL;

    m = CPPALMutexCreate(pal);
    CU_ASSERT(m != NULL);
    CPPALMutexLock(m);
    CPPALMutexUnlock(m);
    CPPALMutexDestroy(m);
    m = NULL;

    // TODO: test contention

    CPRelease(pal);
}

volatile int _test_pal_condition_done = 0;
typedef struct test_pal_condition_state_t {
    CPPALCondition*     c;
} test_pal_condition_state;
sal_callback void test_pal_condition_entry(sal_inout CPPAL* pal, sal_inout_opt void* state)
{
    test_pal_condition_state* cs = (test_pal_condition_state*)state;
    CPPALThreadSleep(pal, 1.0);
    CPAtomicInc32(&_test_pal_condition_done);
    CPPALConditionBroadcast(cs->c);
}
void test_pal_condition()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CPPALCondition* c;
    CPPALMutex* m;

    c = CPPALConditionCreate(pal);
    CU_ASSERT(c != NULL);
    CPPALConditionDestroy(c);
    c = NULL;

    _test_pal_condition_done = 0;
    c = CPPALConditionCreate(pal);
    CU_ASSERT(c != NULL);
    m = CPPALMutexCreate(pal);
    CU_ASSERT(m != NULL);
    test_pal_condition_state cs;
    cs.c    = c;
    CPPALThread* thread = CPPALThreadCreate(pal, CPPALThreadOptionsDefault, test_pal_condition_entry, &cs);
    CU_ASSERT(thread != NULL);
    CPPALMutexLock(m);
    CU_ASSERT_TRUE(CPPALConditionWait(c, m));
    CU_ASSERT(_test_pal_condition_done == 1);
    CPPALMutexUnlock(m);
    CPPALConditionDestroy(c);
    CPPALMutexDestroy(m);
    c = NULL;
    m = NULL;

    // TODO: test multiple waiters

    // TODO: test contention

    CPRelease(pal);
}

void test_pal_heap()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CPPALHeap* heap;

    heap = CPPALHeapCreate(pal, CPPALHeapOptionsDefault, 0);
    CU_ASSERT(heap != NULL);
    CPPALHeapDestroy(heap);
    heap = NULL;

    heap = CPPALHeapCreate(pal, CPPALHeapOptionsDefault, 1024);
    CU_ASSERT(heap != NULL);
    CPPALHeapDestroy(heap);
    heap = NULL;

    heap = CPPALHeapCreate(pal, CPPALHeapOptionsDefault, 8 * 1024 * 1024);
    CU_ASSERT(heap != NULL);
    CPPALHeapDestroy(heap);
    heap = NULL;

    heap = CPPALHeapCreate(pal, CPPALHeapOptionsFixedSize, 0);
    CU_ASSERT(heap != NULL);
    CPPALHeapDestroy(heap);
    heap = NULL;

    heap = CPPALHeapCreate(pal, CPPALHeapOptionsFixedSize, 1024);
    CU_ASSERT(heap != NULL);
    CPPALHeapDestroy(heap);
    heap = NULL;

    heap = CPPALHeapCreate(pal, CPPALHeapOptionsFixedSize, 8 * 1024 * 1024);
    CU_ASSERT(heap != NULL);
    CPPALHeapDestroy(heap);
    heap = NULL;

    // Any way to test this?
    heap = CPPALHeapCreate(pal, CPPALHeapOptionsLockless, 1024);
    CU_ASSERT(heap != NULL);
    CPPALHeapDestroy(heap);
    heap = NULL;

    CPRelease(pal);
}

void test_pal_heap_alloc()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CPPALHeap* heap;
    void* p;

    heap = CPPALHeapCreate(pal, CPPALHeapOptionsDefault, 0);
    CU_ASSERT(heap != NULL);

    p = CPPALHeapAlloc(heap, 0);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 0);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 1);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1024);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1024 * 1024);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 8 * 1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 8 * 1024 * 1024);
    CPPALHeapFree(heap, p);

    CPPALHeapDestroy(heap);
    CPRelease(pal);
}

void test_pal_heap_realloc()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CPPALHeap* heap;
    void* p;
    void* o;

    heap = CPPALHeapCreate(pal, CPPALHeapOptionsDefault, 0);
    CU_ASSERT(heap != NULL);

    p = CPPALHeapRealloc(heap, NULL, 0, 0);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 0);
    CPPALHeapFree(heap, p);

    p = CPPALHeapRealloc(heap, NULL, 0, 1);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1);
    CPPALHeapFree(heap, p);

    p = CPPALHeapRealloc(heap, NULL, 0, 8 * 1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 8 * 1024 * 1024);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 0);
    CU_ASSERT(p != NULL);
    p = CPPALHeapRealloc(heap, p, 0, 0);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 0);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 0);
    CU_ASSERT(p != NULL);
    p = CPPALHeapRealloc(heap, p, 0, 1);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 0);
    CU_ASSERT(p != NULL);
    p = CPPALHeapRealloc(heap, p, 0, 8 * 1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 8 * 1024 * 1024);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 1);
    CU_ASSERT(p != NULL);
    p = CPPALHeapRealloc(heap, p, 1, 0);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 0);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 1);
    CU_ASSERT(p != NULL);
    p = CPPALHeapRealloc(heap, p, 1, 1);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 0);
    CU_ASSERT(p != NULL);
    o = p;
    p = CPPALHeapRealloc(heap, p, 0, 0);
    CU_ASSERT(p != NULL);
    CU_ASSERT(p == o);
    VERIFY_ZEROS(p, 0, 0);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 1);
    CU_ASSERT(p != NULL);
    o = p;
    p = CPPALHeapRealloc(heap, p, 1, 1);
    CU_ASSERT(p != NULL);
    CU_ASSERT(p == o);
    VERIFY_ZEROS(p, 0, 1);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 1024);
    CU_ASSERT(p != NULL);
    p = CPPALHeapRealloc(heap, p, 1024, 1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_ZEROS(p, 0, 1024 * 1024);
    CPPALHeapFree(heap, p);

    p = CPPALHeapAlloc(heap, 1024);
    CU_ASSERT(p != NULL);
    FILL_DC(p, 0, 1024);
    p = CPPALHeapRealloc(heap, p, 1024, 1024 * 1024);
    CU_ASSERT(p != NULL);
    VERIFY_DC(p, 0, 1024);
    VERIFY_ZEROS(p, 1024, 1024 * 1024 - 1024);
    CPPALHeapFree(heap, p);

    CPPALHeapDestroy(heap);
    CPRelease(pal);
}

void test_pal_heap_free()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CPPALHeap* heap;
    void* p;

    heap = CPPALHeapCreate(pal, CPPALHeapOptionsDefault, 0);
    CU_ASSERT(heap != NULL);

    p = CPPALHeapAlloc(heap, 0);
    CU_ASSERT(p != NULL);
    CPPALHeapFree(heap, p);
    p = NULL;

    p = CPPALHeapAlloc(heap, 1024);
    CU_ASSERT(p != NULL);
    CPPALHeapFree(heap, p);
    p = NULL;

    CPPALHeapDestroy(heap);

    CPRelease(pal);
}

void test_pal_heap_fixed()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CPPALHeap* heap;
    void* p;
    void* e;

    heap = CPPALHeapCreate(pal, CPPALHeapOptionsFixedSize, 0);
    CU_ASSERT(heap != NULL);
    p = CPPALHeapAlloc(heap, 0);
    CU_ASSERT(p != NULL);
    CPPALHeapFree(heap, p);
    p = NULL;
    p = CPPALHeapAlloc(heap, 1);
    CU_ASSERT(p == NULL);
    CPPALHeapDestroy(heap);

    heap = CPPALHeapCreate(pal, CPPALHeapOptionsFixedSize, 1024);
    CU_ASSERT(heap != NULL);
    p = CPPALHeapAlloc(heap, 0);
    CU_ASSERT(p != NULL);
    CPPALHeapFree(heap, p);
    p = NULL;
    p = CPPALHeapAlloc(heap, 1);
    CU_ASSERT(p != NULL);
    CPPALHeapFree(heap, p);
    p = NULL;
    p = CPPALHeapAlloc(heap, 1024);
    CU_ASSERT(p != NULL);
    CPPALHeapFree(heap, p);
    p = NULL;
    p = CPPALHeapAlloc(heap, 1025);
    CU_ASSERT(p == NULL);
    p = CPPALHeapAlloc(heap, 1000);
    CU_ASSERT(p != NULL);
    e = CPPALHeapAlloc(heap, 1000);
    CU_ASSERT(e == NULL);
    CPPALHeapFree(heap, p);
    CPPALHeapDestroy(heap);
    heap = NULL;

    CPRelease(pal);
}

typedef struct test_pal_thread_join_state_t {
    uint32          someValue;
} test_pal_thread_join_state;
sal_callback void test_pal_thread_join_entry(sal_inout CPPAL* pal, void* state)
{
    CU_ASSERT(state != NULL);
    test_pal_thread_join_state* p = (test_pal_thread_join_state*)state;
    CPPALThreadSleep(pal, 1.0);
    p->someValue = 456;
}
void test_pal_thread_join()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);
    
    CPPALThread* thread;

    test_pal_thread_join_state state;
    state.someValue = 123;
    thread = CPPALThreadCreate(pal, CPPALThreadOptionsJoinable, test_pal_thread_join_entry, &state);
    CU_ASSERT(thread != NULL);
    CPTime t0 = CPPALAbsoluteTimeGetCurrent(pal);
    CU_ASSERT_TRUE(CPPALThreadJoin(pal, thread));
    CPTime t1 = CPPALAbsoluteTimeGetCurrent(pal);
    CU_ASSERT(t1 - t0 >= 0.9);
    CU_ASSERT(state.someValue == 456);

    CPRelease(pal);
}

typedef struct test_pal_thread_shutdown_state_t {
    CPPALMutex*         m;
    CPPALCondition*     c;
    volatile int        shutdown;
    volatile int        alive;
} test_pal_thread_shutdown_state;
sal_callback void test_pal_thread_shutdown_entry(sal_inout CPPAL* pal, sal_inout_opt void* state)
{
    test_pal_thread_shutdown_state* p = (test_pal_thread_shutdown_state*)state;
    CPPALMutexLock(p->m);
    while (!p->shutdown) {
        CPPALThreadSleep(pal, 0.1);
        CPPALConditionWait(p->c, p->m);
    }
    CPPALMutexUnlock(p->m);
    CPAtomicDec32(&p->alive);
}
void test_pal_thread_shutdown()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);
    
    CPPALThread* thread;
    
    test_pal_thread_shutdown_state state;
    state.m         = CPPALMutexCreate(pal);
    state.c         = CPPALConditionCreate(pal);
    state.shutdown  = 0;
    state.alive     = 1;
    thread = CPPALThreadCreate(pal, CPPALThreadOptionsDefault, test_pal_thread_shutdown_entry, &state);
    CU_ASSERT(thread != NULL);
    CPPALThreadSleep(pal, 0.5);
    CPAtomicInc32(&state.shutdown);
    CPPALThreadShutdown(pal, thread, state.c);
    CU_ASSERT(state.alive == 0);
    CPPALConditionDestroy(state.c);
    CPPALMutexDestroy(state.m);
    
    CPRelease(pal);
}

void test_pal_thread_shutdownlist()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

#define THREAD_COUNT 16
    CPPALThread* threads[THREAD_COUNT];
    
    test_pal_thread_shutdown_state state;
    state.m         = CPPALMutexCreate(pal);
    state.c         = CPPALConditionCreate(pal);
    state.shutdown  = 0;
    state.alive     = 0;
    for (int n = 0; n < THREAD_COUNT; n++) {
        CPAtomicInc32(&state.alive);
        threads[n] = CPPALThreadCreate(pal, CPPALThreadOptionsDefault, test_pal_thread_shutdown_entry, &state);
        CU_ASSERT(threads[n] != NULL);
    }
    CPPALThreadSleep(pal, 0.5);
    CPAtomicInc32(&state.shutdown);
    CPPALThreadShutdownList(pal, threads, CPCOUNT(threads), state.c);
    CU_ASSERT(state.alive == 0);
    CPPALConditionDestroy(state.c);
    CPPALMutexDestroy(state.m);

    CPRelease(pal);
}

typedef struct test_pal_thread_state_t {
    uint32          someValue;
} test_pal_thread_state;
sal_callback void test_pal_thread_no_state_entry(sal_inout CPPAL* pal, sal_inout_opt void* state)
{
    CU_ASSERT(state == NULL);
}
sal_callback void test_pal_thread_state_entry(sal_inout CPPAL* pal, sal_inout_opt void* state)
{
    CU_ASSERT(state != NULL);
    test_pal_thread_state* p = (test_pal_thread_state*)state;
    CU_ASSERT(p->someValue == 123);
}
void test_pal_thread_states()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CPPALThread* thread;

    thread = CPPALThreadCreate(pal, CPPALThreadOptionsJoinable, test_pal_thread_no_state_entry, NULL);
    CU_ASSERT(thread != NULL);
    CU_ASSERT_TRUE(CPPALThreadJoin(pal, thread));

    test_pal_thread_state state;
    state.someValue = 123;
    thread = CPPALThreadCreate(pal, CPPALThreadOptionsJoinable, test_pal_thread_state_entry, &state);
    CU_ASSERT(thread != NULL);
    CU_ASSERT_TRUE(CPPALThreadJoin(pal, thread));

    CPRelease(pal);
}

int test_pal_recursive(int n)
{
    if (n > 0) {
        byte buffer[2048];
        CPZeroMemory(buffer, sizeof(buffer), 0, sizeof(buffer));
        return test_pal_recursive(n - 1);
    } else {
        return 41902;
    }
}
sal_callback void test_pal_thread_smallstack_entry(sal_inout CPPAL* pal, sal_inout_opt void* state)
{
    CU_ASSERT(state == NULL);
    test_pal_recursive(10);
}
void test_pal_thread_smallstack()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CPPALThread* thread;

    thread = CPPALThreadCreate(pal, CPPALThreadOptionsJoinable | CPPALThreadOptionsSmallStack, test_pal_thread_smallstack_entry, NULL);
    CU_ASSERT(thread != NULL);
    CU_ASSERT_TRUE(CPPALThreadJoin(pal, thread));

    CPRelease(pal);
}

sal_callback void test_pal_thread_ismain_entry(sal_inout CPPAL* pal, sal_inout_opt void* state)
{
    CU_ASSERT_FALSE(CPPALThreadIsMain(pal));
}
void test_pal_thread_ismain()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CU_ASSERT_TRUE(CPPALThreadIsMain(pal));

    CPPALThread* thread = CPPALThreadCreate(pal, CPPALThreadOptionsJoinable, test_pal_thread_ismain_entry, NULL);
    CU_ASSERT(thread != NULL);

    CPIGNORE(CPPALThreadJoin(pal, thread));
    thread = NULL;

    CPRelease(pal);
}

void* _test_pal_threadid = NULL;
sal_callback void test_pal_thread_getid_entry(sal_inout CPPAL* pal, sal_inout_opt void* state)
{
    CU_ASSERT_NOT_EQUAL(CPPALThreadGetID(pal), _test_pal_threadid);
}
void test_pal_thread_getid()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    _test_pal_threadid = CPPALThreadGetID(pal);
    CU_ASSERT(_test_pal_threadid != NULL);

    CPPALThread* thread = CPPALThreadCreate(pal, CPPALThreadOptionsJoinable, test_pal_thread_getid_entry, NULL);
    CU_ASSERT(thread != NULL);

    CPIGNORE(CPPALThreadJoin(pal, thread));
    thread = NULL;

    CPRelease(pal);
}

void test_pal_sleep()
{
    CPPALOptions options;
    CPPALOptionsInitialize(&options);
    CPPAL* pal = CPPALCreate(&options);

    CPTime t0;
    CPTime t1;

    t0 = CPPALAbsoluteTimeGetCurrent(pal);
    CPPALThreadSleep(pal, 1.0);
    t1 = CPPALAbsoluteTimeGetCurrent(pal);
    CU_ASSERT(t1 - t0 >= 0.9);
    CU_ASSERT(t1 - t0 < 1.5);

    t0 = CPPALAbsoluteTimeGetCurrent(pal);
    CPPALThreadSleep(pal, 0.0);
    t1 = CPPALAbsoluteTimeGetCurrent(pal);
    CU_ASSERT(t1 - t0 >= 0.0);
    CU_ASSERT(t1 - t0 < 0.1);

    CPRelease(pal);
}

static CU_TestInfo test_pal_infos[] = {
    { "test_pal_creation",          test_pal_creation           },
    { "test_pal_systeminfo",        test_pal_systeminfo         },
    { "test_pal_systempaths",       test_pal_systempaths        },
    { "test_pal_pathconversion",    test_pal_pathconversion     },
    { "test_pal_time",              test_pal_time               },
    { "test_pal_random",            test_pal_random             },
    { "test_pal_mutex",             test_pal_mutex              },
    { "test_pal_condition",         test_pal_condition          },
    { "test_pal_heap",              test_pal_heap               },
    { "test_pal_heap_alloc",        test_pal_heap_alloc         },
    { "test_pal_heap_realloc",      test_pal_heap_realloc       },
    { "test_pal_heap_free",         test_pal_heap_free          },
    { "test_pal_heap_fixed",        test_pal_heap_fixed         },
    { "test_pal_thread_join",       test_pal_thread_join        },
    { "test_pal_thread_shutdown",   test_pal_thread_shutdown    },
    { "test_pal_thread_shutdownlist", test_pal_thread_shutdownlist },
    { "test_pal_thread_states",     test_pal_thread_states      },
    { "test_pal_thread_smallstack", test_pal_thread_smallstack  },
    { "test_pal_thread_ismain",     test_pal_thread_ismain      },
    { "test_pal_thread_getid",      test_pal_thread_getid       },
    { "test_pal_sleep",             test_pal_sleep              },
};

CU_REGISTER_SUITE(test_pal);
