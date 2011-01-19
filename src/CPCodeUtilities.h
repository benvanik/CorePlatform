/*
 *  CPCodeUtilities.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CPPlatform.h>
#include <CPTypes.h>

#define CP_EMPTY_MACRO                  do { } while(0)

#define CPUNREFERENCED(expr)            (void)(expr)

#if CP_COMPILER(MSVC)
#define CPASSUME(expr)                  __analysis_assume(expr)
#else
#define CPASSUME(expr)                  
#endif

#if CP_COMPILER(MSVC)
// http://msdn.microsoft.com/en-us/library/z8y1yy88.aspx
#define CPFORCEINLINE                   static __forceinline
#define CPNOINLINE                      __declspec(noinline)
#elif CP_COMPILER(GNUC)
// http://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
#if (__GNUC__ >= 4)
#define CPFORCEINLINE                   static __inline__ __attribute__ ((always_inline))
#else
#define CPFORCEINLINE                   static __inline__
#endif
#define CPNOINLINE                      
#else
#define CPFORCEINLINE                   
#define CPNOINLINE                      
#endif
// In debug mode always disable inlines (I'll thank myself for this later)
#if defined(CP_DEBUG)
#undef CPFORCEINLINE
#define CPFORCEINLINE
#endif

#if CP_COMPILER(MSVC)
// http://msdn.microsoft.com/en-us/library/83ythb65.aspx
#define CPCACHEALIGN                    __declspec(align(CP_ALIGNMENT))
#elif CP_COMPILER(GNUC)
// http://gcc.gnu.org/onlinedocs/gcc/Type-Attributes.html
#define CPCACHEALIGN                    __attribute__ ((aligned(CP_ALIGNMENT)))
#else
#define CPCACHEALIGN                    
#endif
typedef CPCACHEALIGN volatile void      CPAlignedVoid;

#if CP_COMPILER(MSVC)
// http://msdn.microsoft.com/en-us/library/ms175773.aspx
#define CPCOUNT(array)                  _countof(array)
#elif CP_COMPILER(GNUC)
#define CPCOUNT(array)                  (sizeof(array) / sizeof(__typeof__(array[0])))
#else
#define CPCOUNT(array)                  (sizeof(array) / sizeof(array[0]))
#endif

#if defined(__cplusplus)
#define CP_EXTERNC                      extern "C"
#else
#define CP_EXTERNC                      extern
#endif
#if CP_COMPILER(MSVC)
#if defined(CP_API_EXPORT)
#define CP_API                          CP_EXTERNC __declspec(dllexport)
#elif defined(CP_API_IMPORT)
#define CP_API                          CP_EXTERNC __declspec(dllimport)
#else
#define CP_API                          CP_EXTERNC
#endif
#else
#define CP_API                          CP_EXTERNC
#endif

#if !defined(MIN)
#if CP_COMPILER(GNUC)
#define MIN(A, B)                       ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); (__a < __b) ? __a : __b; })
#define MAX(A, B)                       ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); (__a < __b) ? __b : __a; })
//#elif CP_COMPILER(MSVC)
// TODO: experiment with decltype -- http://msdn.microsoft.com/en-us/library/dd537655.aspx
#else
// NOTE: this implementation will evaluate the arguments twice - may be worth writing an inline function instead
#define MIN(A, B)                       ( ((A) < (B)) ? (A) : (B) )
#define MAX(A, B)                       ( ((A) < (B)) ? (B) : (A) )
#endif
#endif

#define CPEXPECT(expr, result)          if ( (expr) != (result) ) { goto CPCLEANUP; }
#define CPEXPECTNOT(expr, result)       if ( (expr) == (result) ) { goto CPCLEANUP; }
#define CPEXPECTTRUE(expr)              if (!(expr)             ) { goto CPCLEANUP; }
#define CPEXPECTFALSE(expr)             if ( (expr)             ) { goto CPCLEANUP; }
#define CPEXPECTZERO(expr)              if ( (expr) != 0        ) { goto CPCLEANUP; }
#define CPEXPECTNOTZERO(expr)           if ( (expr) == 0        ) { goto CPCLEANUP; }
#define CPEXPECTNULL(expr)              if ( (expr) != NULL     ) { goto CPCLEANUP; }
#define CPEXPECTNOTNULL(expr)           if ( (expr) == NULL     ) { goto CPCLEANUP; }
#define CPIGNORE(expr)                  do { (void)(expr); } while(0)

