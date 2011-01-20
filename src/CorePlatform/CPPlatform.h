/*
 *  CPPlatform.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

// NOTE: ordering matters here as sometimes multiple flags are defined on certain platforms

// Great resource on predefined macros: http://predef.sourceforge.net/preos.html

/*
CP_PLATFORM:    IOS | OSX | XBOX360 | WINCE | WIN32 | ANDROID | NACL | UNIX
CP_LIKE:        OSX | WIN32 | POSIX
CP_PROFILE:     EMBEDDED | DESKTOP (+ _SIMULATOR)
CP_COMPILER:    GNUC | MSVC | INTEL | UNKNOWN
CP_CPU:         32BIT | 64BIT | BIGENDIAN | LITTLEENDIAN
*/

#define CP_PLATFORM(NAME)   (defined CP_PLATFORM_##NAME && CP_PLATFORM_##NAME   )
#define CP_LIKE(NAME)       (defined CP_LIKE_##NAME     && CP_LIKE_##NAME       )
#define CP_PROFILE(NAME)    (defined CP_PROFILE_##NAME  && CP_PROFILE_##NAME    )
#define CP_COMPILER(NAME)   (defined CP_COMPILER_##NAME && CP_COMPILER_##NAME   )
#define CP_CPU(NAME)        (defined CP_CPU_##NAME      && CP_CPU_##NAME        )

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#if ( defined(TARGET_OS_EMBEDDED)       && TARGET_OS_EMBEDDED       ) || \
    ( defined(TARGET_OS_IPHONE)         && TARGET_OS_IPHONE         ) || \
    ( defined(TARGET_IPHONE_SIMULATOR)  && TARGET_IPHONE_SIMULATOR  )

#define CP_PLATFORM_IOS         1
#define CP_LIKE_OSX             1
#define CP_PROFILE_EMBEDDED     1

#if defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR
// EMBEDDED *and* SIMULATOR
#define CP_PROFILE_SIMULATOR    1
#endif

#elif defined(TARGET_OS_MAC) && TARGET_OS_MAC

#define CP_PLATFORM_OSX         1
#define CP_LIKE_OSX             1
#define CP_PROFILE_DESKTOP      1

#elif defined(_XBOX)

#define CP_PLATFORM_XBOX360     1
#define CP_LIKE_WIN32           1
#define CP_PROFILE_EMBEDDED     1

#elif defined(_WIN32_WCE)

#define CP_PLATFORM_WINCE       1
#define CP_LIKE_WIN32           1
#define CP_PROFILE_EMBEDDED     1

#elif defined(__CYGWIN__)

#define CP_PLATFORM_CYGWIN      1
#define CP_LIKE_POSIX           1
#define CP_PROFILE_DESKTOP      1

#elif defined(WIN32) || defined(_WIN32)

#define CP_PLATFORM_WIN32       1
#define CP_LIKE_WIN32           1
#define CP_PROFILE_DESKTOP      1

#elif defined(ANDROID)

#define CP_PLATFORM_ANDROID     1
#define CP_LIKE_POSIX           1
#define CP_PROFILE_EMBEDDED     1

#elif defined(__native_client__)

#define CP_PLATFORM_NACL        1
#define CP_LIKE_POSIX           1
#define CP_PROFILE_DESKTOP      1

#else

#define CP_PLATFORM_UNIX        1
#define CP_LIKE_POSIX           1
#define CP_PROFILE_DESKTOP      1

#endif

#if defined(__GNUC__)
#define CP_COMPILER_GNUC        1
#elif defined(_MSC_VER)
#define CP_COMPILER_MSVC        1
#elif defined(__MINGW32)
#define CP_COMPILER_MINGW32     1
#elif defined(__INTEL_COMPILER)
#define CP_COMPILER_INTEL       1
#else
#define CP_COMPILER_UNKNOWN     1
#endif

#if defined(__ia64__) || defined(_M_IA64) || \
    defined(__ppc64__) || defined(__PPC64__) || \
    defined(__arch64__) || \
    defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) || \
    defined(__LP64__) || defined(__LLP64) || \
    defined(_WIN64) || \
    (__WORDSIZE == 64)
#define CP_CPU_64BIT            1
#else
#define CP_CPU_32BIT            1
#endif

#if defined(__ppc__) || defined(__PPC__) || defined(__powerpc__) || defined(__powerpc) || defined(__POWERPC__) || defined(_M_PPC) || defined(__PPC) || \
    defined(__ppc64__) || defined(__PPC64__) || \
    defined(__ARMEB__) || \
    defined(__BIG_ENDIAN) || defined(__BIG_ENDIAN__)
#define CP_CPU_BIGENDIAN        1
#else
#define CP_CPU_LITTLEENDIAN     1
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define CP_DEBUG                1
#endif

#if CP_CPU(32BIT)
#define CP_ALIGNMENT            8
#else
#define CP_ALIGNMENT            16
#endif




#if CP_PLATFORM(WINCE) || CP_PLATFORM(WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT            0x0601
#endif
#include <windows.h>
#endif // WINCE || WIN32

#if CP_PLATFORM(XBOX360)
#include <xtl.h>
#include <xboxmath.h>
#endif // XBOX360

#if CP_COMPILER(MSVC)
// Disable warning C4068: unknown pragma
#pragma warning(disable : 4068)
#endif // MSVC

#include <stdlib.h>
