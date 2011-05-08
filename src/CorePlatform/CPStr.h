/*
 *  CPStr.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CorePlatform/CPPlatform.h>
#include <CorePlatform/CPConfig.h>
#include <CorePlatform/CPAnnotations.h>
#include <CorePlatform/CPCodeUtilities.h>
#include <CorePlatform/CPMemory.h>

#include <stdio.h>
#include <stdarg.h>

// NOTE: these differing implementations should behave pretty much the same - if they don't, then they will have to be abstracted out

#define CPInvalidSize                                   ((size_t)(-1))

#if !CP_LIKE(WIN32)
#define strcpy_s(dest, destLength, source)              !(strcpy(dest, source) == dest + (destLength*0))
#define strncpy_s(dest, destLength, source, count)      !(strncpy(dest, source, MIN(destLength, count)) == dest + (destLength*0))
#define strcat_s(dest, destLength, source)              !(strcat(dest, source) == dest + (destLength*0))
#else
#define strcasecmp                                      _stricmp
#endif

#define CPStrLenW                                       wcslen
#define CPStrCmpW                                       wcscmp
#define CPStrCaseCmpW                                   _wcsicmp
#define CPStrChrW                                       wcschr
#define CPStrRChrW                                      wcsrchr
#define CPStrStrW                                       wcsstr
#define CPStrCpyW(dest, destLength, source)             (wcscpy_s(dest, destLength, source) == 0)
#define CPStrNCpyW(dest, destLength, source, count)     (wcsncpy_s(dest, destLength, source, count) == 0)
#define CPStrCatW(dest, destLength, source)             (wcscat_s(dest, destLength, source) == 0)
#define CPSNPrintFW(buffer, bufferCount, format, ...)   _snwprintf_s(buffer, bufferCount, (bufferCount) ? (bufferCount - 1) : 0, format, ##__VA_ARGS__)
#define CPVSNPrintFW(buffer, bufferCount, format, args) _vsnwprintf_s(buffer, bufferCount, (bufferCount) ? (bufferCount - 1) : 0, format, args)
#define CPVSCPrintFW(format, args)                      _vscwprintf(format, args)

#define CPStrLenA                                       strlen
#define CPStrCmpA                                       strcmp
#define CPStrCaseCmpA                                   strcasecmp
#define CPStrChrA                                       strchr
#define CPStrRChrA                                      strrchr
#define CPStrStrA                                       strstr
#define CPStrCpyA(dest, destLength, source)             (strcpy_s(dest, destLength, source) == 0)
#define CPStrNCpyA(dest, destLength, source, count)     (strncpy_s(dest, destLength, source, count) == 0)
#define CPStrCatA(dest, destLength, source)             (strcat_s(dest, destLength, source) == 0)
#define CPSNPrintFA(buffer, bufferCount, format, ...)   snprintf(buffer, bufferCount, format, ##__VA_ARGS__)
#define CPVSNPrintFA(buffer, bufferCount, format, args) vsnprintf(buffer, bufferCount, format, args)
#undef CPVSCPrintFA     // Not needed

#if CP_PLATFORM(WIN32)

typedef wchar_t CPChar;
#define CP_WCHAR            1
#define CPTEXT(s)           L ## s

#define CPStrLen            CPStrLenW
#define CPStrCmp            CPStrCmpW
#define CPStrCaseCmp        CPStrCaseCmpW
#define CPStrChr            CPStrChrW
#define CPStrRChr           CPStrRChrW
#define CPStrStr            CPStrStrW
#define CPStrCpy            CPStrCpyW
#define CPStrNCpy           CPStrNCpyW
#define CPStrCat            CPStrCatW
#define CPSNPrintF          CPSNPrintFW
#define CPVSNPrintF         CPVSNPrintFW

#define CPStrWiden(dest, destLength, source)    mbstowcs_s(NULL, dest, destLength, source, _TRUNCATE)

#else

typedef char CPChar;
#define CP_CHAR             1
#define CPTEXT(s)           s

#define CPStrLen            CPStrLenA
#define CPStrCmp            CPStrCmpA
#define CPStrCaseCmp        CPStrCaseCmpA
#define CPStrChr            CPStrChrA
#define CPStrRChr           CPStrRChrA
#define CPStrStr            CPStrStrA
#define CPStrCpy            CPStrCpyA
#define CPStrNCpy           CPStrNCpyA
#define CPStrCat            CPStrCatA
#define CPSNPrintF          CPSNPrintFA
#define CPVSNPrintF         CPVSNPrintFA

#define CPStrWiden(dest, destLength, source)    CPStrCpy(dest, destLength, source)

#endif // WIN32
