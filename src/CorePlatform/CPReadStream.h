/*
 *  CPReadStream.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 5/08/11.
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

typedef enum CPSeekOrigin_e {
    CPSeekOriginSet             = 0,
    CPSeekOriginCurrent         = 1,
    CPSeekOriginEnd             = 2,
} CPSeekOrigin;

CP_DECLARE_TYPE(CPReadStream);

struct CPReadStream_t {
    CPObject        object;

    BOOL            closed;

    void (*close)(sal_inout CPReadStreamRef stream);

    size_t (*getLength)(sal_inout CPReadStreamRef stream);
    BOOL (*isEOF)(sal_inout CPReadStreamRef stream);

    size_t (*getPosition)(sal_inout CPReadStreamRef stream);
    void (*seek)(sal_inout CPReadStreamRef stream, const size_t offset, const CPSeekOrigin origin);

    size_t (*readBuffer)(sal_inout CPReadStreamRef stream, sal_out_bcount_opt(bufferSize) byte* buffer, const size_t bufferSize);
    size_t (*readSection)(sal_inout CPReadStreamRef stream, const size_t offset, sal_out_bcount_opt(bufferSize) byte* buffer, const size_t bufferSize);
};

CP_API sal_checkReturn sal_out_opt CPReadStreamRef CPReadStreamAlloc(sal_in const CPObjectType* subclassType, const size_t size);

CP_API void CPReadStreamClose(sal_inout CPReadStreamRef stream);

CP_API size_t CPReadStreamGetLength(sal_inout CPReadStreamRef stream);
CP_API BOOL CPReadStreamIsEOF(sal_inout CPReadStreamRef stream);

CP_API size_t CPReadStreamGetPosition(sal_inout CPReadStreamRef stream);
CP_API void CPReadStreamSetPosition(sal_inout CPReadStreamRef stream, const size_t position);
CP_API void CPReadStreamSeek(sal_inout CPReadStreamRef stream, const size_t offset, const CPSeekOrigin origin);

CP_API size_t CPReadStreamReadBuffer(sal_inout CPReadStreamRef stream, sal_out_bcount_opt(bufferSize) byte* buffer, const size_t bufferSize);
CP_API size_t CPReadStreamReadSection(sal_inout CPReadStreamRef stream, const size_t offset, sal_out_bcount_opt(bufferSize) byte* buffer, const size_t bufferSize);
