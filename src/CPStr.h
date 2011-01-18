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

#define CPSNPrintF(buffer, bufferCount, format, ...)    _snwprintf_s(buffer, bufferCount, bufferCount, format, ##__VA_ARGS__)
#define CPVSNPrintF(buffer, bufferCount, format, args)  _vsnwprintf_s(buffer, bufferCount, bufferCount, format, args)

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

#define CPSNPrintF(buffer, bufferCount, format, ...)    snprintf(buffer, bufferCount, format, ##__VA_ARGS__)
#define CPVSNPrintF(buffer, bufferCount, format, args)  vsnprintf(buffer, bufferCount, format, args)

#endif // WIN32
