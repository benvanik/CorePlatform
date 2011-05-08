/*
 *  CPReadStream.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 5/08/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPReadStream.h>

CP_DEFINE_TYPE(CPReadStream, NULL, CPReadStreamDealloc);

CP_API sal_checkReturn sal_out_opt CPReadStreamRef CPReadStreamAlloc(sal_in const CPObjectType* subclassType, const size_t size)
{
    CPReadStreamRef stream = (CPReadStreamRef)CPObjectAlloc(subclassType, size);
    CPEXPECTNOTNULL(stream);

    stream->closed      = FALSE;

    return stream;

CPCLEANUP:
    CPRelease(stream);
    return NULL;
}

sal_callback void CPReadStreamDealloc(sal_inout CPReadStreamRef stream)
{
    CPReadStreamClose(stream);
}

CP_API void CPReadStreamClose(sal_inout CPReadStreamRef stream)
{
    if (stream->closed) {
        return;
    }

    if (stream->close) {
        stream->close(stream);
    }
}

CP_API size_t CPReadStreamGetLength(sal_inout CPReadStreamRef stream)
{
    if (stream->closed) {
        return CPInvalidSize;
    }

    if (stream->getLength) {
        return stream->getLength(stream);
    } else {
        return CPInvalidSize;
    }
}

CP_API BOOL CPReadStreamIsEOF(sal_inout CPReadStreamRef stream)
{
    if (stream->closed) {
        return TRUE;
    }

    if (stream->isEOF) {
        return stream->isEOF(stream);
    } else {
        return FALSE;
    }
}

CP_API size_t CPReadStreamGetPosition(sal_inout CPReadStreamRef stream)
{
    if (stream->closed) {
        return CPInvalidSize;
    }

    if (stream->getPosition) {
        return stream->getPosition(stream);
    } else {
        return CPInvalidSize;
    }
}

CP_API void CPReadStreamSetPosition(sal_inout CPReadStreamRef stream, const size_t position)
{
    if (stream->closed) {
        return;
    }

    if (stream->seek) {
        stream->seek(stream, position, CPSeekOriginSet);
    }
}

CP_API void CPReadStreamSeek(sal_inout CPReadStreamRef stream, const size_t offset, const CPSeekOrigin origin)
{
    if (stream->closed) {
        return;
    }

    if (stream->seek) {
        stream->seek(stream, offset, origin);
    }
}

CP_API size_t CPReadStreamReadBuffer(sal_inout CPReadStreamRef stream, sal_out_bcount_opt(bufferSize) byte* buffer, const size_t bufferSize)
{
    if (stream->closed) {
        return CPInvalidSize;
    }

    if (stream->readBuffer) {
        return stream->readBuffer(stream, buffer, bufferSize);
    } else {
        return CPInvalidSize;
    }
}

CP_API size_t CPReadStreamReadSection(sal_inout CPReadStreamRef stream, const size_t offset, sal_out_bcount_opt(bufferSize) byte* buffer, const size_t bufferSize)
{
    if (stream->closed) {
        return CPInvalidSize;
    }

    if (stream->readSection) {
        return stream->readSection(stream, offset, buffer, bufferSize);
    } else {
        return CPInvalidSize;
    }
}
