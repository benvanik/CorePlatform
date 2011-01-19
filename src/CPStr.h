/*
 *  CPStr.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/16/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CPPlatform.h>
#include <CPConfig.h>
#include <CPAnnotations.h>
#include <CPCodeUtilities.h>
#include <CPMemory.h>

#include <stdio.h>
#include <stdarg.h>

// NOTE: these differing implementations should behave pretty much the same - if they don't, then they will have to be abstracted out

#if CP_PLATFORM(WIN32)

typedef wchar_t CPChar;
#define CP_WCHAR            1
#define CPTEXT(s)           L ## s

#define CPStrLen            wcslen
#define CPStrCmp            wcscmp
#define CPStrCaseCmp        _wcsicmp
#define CPStrChr            wcschr
#define CPStrRChr           wcsrchr
#define CPStrStr            wcsstr

#define CPStrCpy(dest, destLength, source)              (wcscpy_s(dest, destLength, source) == 0)
#define CPStrNCpy(dest, destLength, source, count)      (wcsncpy_s(dest, destLength, source, count) == 0)
#define CPStrCat(dest, destLength, source)              (wcscat_s(dest, destLength, source) == 0)

#define CPSNPrintF(buffer, bufferCount, format, ...)    _snwprintf_s(buffer, bufferCount, (bufferCount) ? (bufferCount - 1) : 0, format, ##__VA_ARGS__)
#define CPVSNPrintF(buffer, bufferCount, format, args)  _vsnwprintf_s(buffer, bufferCount, (bufferCount) ? (bufferCount - 1) : 0, format, args)

#define CPVSCPrintF(format, args)                       _vscwprintf(format, args)

#else

typedef char CPChar;
#define CP_CHAR             1
#define CPTEXT(s)           s

#define CPStrLen            strlen
#define CPStrCmp            strcmp
#define CPStrCaseCmp        strcasecmp
#define CPStrChr            strchr
#define CPStrRChr           strrchr
#define CPStrStr            strstr

#define CPStrCpy(dest, destLength, source)              (strcpy(dest, source) == dest + (destLength*0))
#define CPStrNCpy(dest, destLength, source, count)      (strncpy(dest, source, count) == dest + (destLength*0))
#define CPStrCat(dest, destLength, source)              (strcat(dest, source) == dest + (destLength*0))

#define CPSNPrintF(buffer, bufferCount, format, ...)    snprintf(buffer, bufferCount, format, ##__VA_ARGS__)
#define CPVSNPrintF(buffer, bufferCount, format, args)  vsnprintf(buffer, bufferCount, format, args)

#undef CPVSCPrintF  // Not needed

#endif // WIN32
