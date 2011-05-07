/*
 *  CPFile.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 5/07/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CorePlatform/CPPlatform.h>
#include <CorePlatform/CPConfig.h>
#include <CorePlatform/CPTypes.h>
#include <CorePlatform/CPAnnotations.h>
#include <CorePlatform/CPCodeUtilities.h>
#include <CorePlatform/CPMath.h>
#include <CorePlatform/CPAtomic.h>
#include <CorePlatform/CPMemory.h>
#include <CorePlatform/CPDebugging.h>
#include <CorePlatform/CPObject.h>
#include <CorePlatform/CPURL.h>
#include <CorePlatform/CPPAL.h>

typedef enum CPFileMode_e {
    CPFileModeRead              = 0x1,
    CPFileModeWrite             = 0x2,
} CPFileMode;

CP_DECLARE_TYPE(CPFile);

struct CPFile_t {
    CPObject        object;

    CPURLRef        path;
    CPFileMode      mode;
    void*           handle;
};

CP_API sal_checkReturn sal_out_opt CPFileRef CPFileCreate(sal_inout CPPALRef pal, sal_inout CPURLRef path);
CP_API sal_checkReturn sal_out_opt CPFileRef CPFileOpen(sal_inout CPPALRef pal, sal_inout CPURLRef path, const CPFileMode mode);
CP_API void CPFileClose(sal_inout CPFileRef file);

CP_API size_t CPFileGetSize(sal_inout CPFileRef file);
//CP_API BOOL CPFileSetSize(sal_inout CPFileRef file, const size_t newSize);

CP_API size_t CPFileRead(sal_inout CPFileRef file, const size_t offset, sal_out_bcount_opt(bufferSize) byte* buffer, const size_t bufferSize);
CP_API BOOL CPFileWrite(sal_inout CPFileRef file, const size_t offset, sal_in_bcount(bufferSize) const byte* buffer, const size_t bufferSize);
