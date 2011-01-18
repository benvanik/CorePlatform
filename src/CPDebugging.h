/*
 *  CPDebugging.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CPPlatform.h>
#include <CPConfig.h>
#include <CPCodeUtilities.h>

#if CP_COMPILER(MSVC)
// http://msdn.microsoft.com/en-us/library/b0084kay.aspx
#if !defined(__WFILE__)
#define WIDEN2(x)       L##x
#define WIDEN(x)        WIDEN2(x)
#define __WFILE__       WIDEN(__FILE__)
#define __WFUNCTION__   WIDEN(__FUNCTION__)
#endif
#define CP_CURRENT_FILE     __WFILE__
#define CP_CURRENT_FUNCTION __WFUNCTION__
#else
#define CP_CURRENT_FILE     __FILE__
#define CP_CURRENT_FUNCTION __FUNCTION__
#endif
#define CP_CURRENT_LINE     __LINE__



#include <assert.h>
#define __CP_ASSERT(expr)               assert(expr)



#if CP_OPTION(ENABLE_ASSERTS)
#define CPASSERTCORE(expr)              __CP_ASSERT(expr)
#else
#define CPASSERTCORE(expr)              CP_EMPTY_MACRO
#endif

#define CPASSERT(expr)                  CPASSERTCORE( (expr)             )
#define CPASSERTTRUE(expr)              CPASSERTCORE( (expr)             )
#define CPASSERTFALSE(expr)             CPASSERTCORE(!(expr)             )
#define CPASSERTZERO(expr)              CPASSERTCORE( (expr) == 0        )
#define CPASSERTNOTZERO(expr)           CPASSERTCORE( (expr) != 0        )
#define CPASSERTNULL(expr)              CPASSERTCORE( (expr) == NULL     )
#define CPASSERTNOTNULL(expr)           CPASSERTCORE( (expr) != NULL     )




#if CP_COMPILER(MSVC)
// http://msdn.microsoft.com/en-us/library/bb918086.aspx
// TODO: if 2010+, use static_assert? http://msdn.microsoft.com/en-us/library/dd293588.aspx
#define CPSTATICASSERT(expr, message)   _STATIC_ASSERT(expr)
//#elif CP_COMPILER(GNUC)
// http://stackoverflow.com/questions/3385515/static-assert-in-c
//#define CPSTATICASSERT(expr, message)   ({ extern int __attribute__((error("assertion failure: '" #expr "' not true - " #message))) compile_time_check(); ((expr)?0:compile_time_check()),0; })
#else
// http://stackoverflow.com/questions/3385515/static-assert-in-c
#define CPSTATICASSERT3(expr, L)        typedef char static_assertion_##L[(expr)?1:-1]
#define CPSTATICASSERT2(expr, L)        CPSTATICASSERT3(expr, L)
#define CPSTATICASSERT(expr, message)   CPSTATICASSERT2(expr, __LINE__)
#endif




#if CP_OPTION(ENABLE_LOGGING)
#define CPLOGCORE(level, tag, fmt, ...) CPPALLogMessage(CP_CURRENT_FILE, CP_CURRENT_LINE, CP_CURRENT_FUNCTION, tag, level, fmt, ##__VA_ARGS__)
#else
#define CPLOGCORE(level, tag, fmt, ...) CP_EMPTY_MACRO
#endif

#if CP_OPTION(LOG_ERROR)
#define CPLOGE(fmt, ...)                CPLOGCORE(CPPALLogLevelError, NULL, fmt, ##__VA_ARGS__)
#define CPLOGTE(tag, fmt, ...)          CPLOGCORE(CPPALLogLevelError, tag, fmt, ##__VA_ARGS__)
#else
#define CPLOGE(fmt, ...)                CP_EMPTY_MACRO
#define CPLOGTE(tag, fmt, ...)          CP_EMPTY_MACRO
#endif
#if CP_OPTION(LOG_WARNING)
#define CPLOGW(fmt, ...)                CPLOGCORE(CPPALLogLevelWarning, NULL, fmt, ##__VA_ARGS__)
#define CPLOGTW(tag, fmt, ...)          CPLOGCORE(CPPALLogLevelWarning, tag, fmt, ##__VA_ARGS__)
#else
#define CPLOGW(fmt, ...)                CP_EMPTY_MACRO
#define CPLOGTW(tag, fmt, ...)          CP_EMPTY_MACRO
#endif
#if CP_OPTION(LOG_INFO)
#define CPLOGI(fmt, ...)                CPLOGCORE(CPPALLogLevelInfo, NULL, fmt, ##__VA_ARGS__)
#define CPLOGTI(tag, fmt, ...)          CPLOGCORE(CPPALLogLevelInfo, tag, fmt, ##__VA_ARGS__)
#else
#define CPLOGI(fmt, ...)                CP_EMPTY_MACRO
#define CPLOGTI(tag, fmt, ...)          CP_EMPTY_MACRO
#endif
#if CP_OPTION(LOG_DEBUG)
#define CPLOGD(fmt, ...)                CPLOGCORE(CPPALLogLevelDebug, NULL, fmt, ##__VA_ARGS__)
#define CPLOGTD(tag, fmt, ...)          CPLOGCORE(CPPALLogLevelDebug, tag, fmt, ##__VA_ARGS__)
#else
#define CPLOGD(fmt, ...)                CP_EMPTY_MACRO
#define CPLOGTD(tag, fmt, ...)          CP_EMPTY_MACRO
#endif
