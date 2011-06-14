/*
 *  CPFileReadStream.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 5/08/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPFileReadStream.h>

// TODO: put all FILE methods on the PAL
#if CP_LIKE(WIN32)
#define fseeko                          _fseeki64
#define ftello                          _ftelli64
#else
#endif

CP_DEFINE_TYPE(CPFileReadStream, NULL, CPFileReadStreamDealloc);

void _CPFileReadStreamSetFunctions(sal_inout CPReadStreamRef stream);

CP_API sal_checkReturn sal_out_opt CPFileReadStreamRef CPFileReadStreamCreateWithPath(sal_inout CPPALRef pal, sal_inout CPURLRef path)
{
    CPChar fullPath[CP_MAX_PATH];
    CPEXPECTTRUE(CPPALConvertURLToFileSystemPath(pal, path, fullPath, CPCOUNT(fullPath)));

    CPFileReadStreamRef stream = (CPFileReadStreamRef)CPReadStreamAlloc(&CPFileReadStreamType, sizeof(CPFileReadStream));
    CPEXPECTNOTNULL(stream);

    stream->pal     = CPPALRetain(pal);
    stream->path    = CPURLRetain(path);

    _CPFileReadStreamSetFunctions(&stream->base);

#if CP_LIKE(WIN32)
    CPEXPECTZERO(_wfopen_s((FILE**)&stream->handle, fullPath, L"rb"));
#else
    stream->handle = fopen(fullPath, "rb");
    CPEXPECTNOTNULL(stream->handle);
#endif

    return stream;

CPCLEANUP:
    CPRelease(stream);
    return NULL;
}

sal_callback void CPFileReadStreamDealloc(sal_inout CPFileReadStreamRef stream)
{
    CPRelease(stream->path);
    CPRelease(stream->pal);
}

sal_callback void CPFileReadStreamClose(sal_inout CPReadStreamRef baseStream)
{
    CPFileReadStreamRef stream = (CPFileReadStreamRef)baseStream;
    FILE* handle = (FILE*)stream->handle;

    if (handle) {
        fclose(handle);
        stream->handle = NULL;
    }

    stream->base.closed = TRUE;
}

sal_callback size_t CPFileReadStreamGetLength(sal_inout CPReadStreamRef baseStream)
{
    CPFileReadStreamRef stream = (CPFileReadStreamRef)baseStream;
    FILE* handle = (FILE*)stream->handle;

    size_t pos = ftello(handle);
    fseeko(handle, 0, SEEK_END);
    size_t size = ftello(handle);
    fseeko(handle, pos, SEEK_SET);
    return size;
}

sal_callback BOOL CPFileReadStreamIsEOF(sal_inout CPReadStreamRef baseStream)
{
    CPFileReadStreamRef stream = (CPFileReadStreamRef)baseStream;
    FILE* handle = (FILE*)stream->handle;

    return feof(handle) != 0;
}

sal_callback size_t CPFileReadStreamGetPosition(sal_inout CPReadStreamRef baseStream)
{
    CPFileReadStreamRef stream = (CPFileReadStreamRef)baseStream;
    FILE* handle = (FILE*)stream->handle;

    return ftello(handle);
}

sal_callback void CPFileReadStreamSeek(sal_inout CPReadStreamRef baseStream, const size_t offset, const CPSeekOrigin origin)
{
    CPFileReadStreamRef stream = (CPFileReadStreamRef)baseStream;
    FILE* handle = (FILE*)stream->handle;

    fseeko(handle, offset, origin);
}

sal_callback size_t CPFileReadStreamReadBuffer(sal_inout CPReadStreamRef baseStream, sal_out_bcount_opt(bufferSize) byte* buffer, const size_t bufferSize)
{
    CPFileReadStreamRef stream = (CPFileReadStreamRef)baseStream;
    FILE* handle = (FILE*)stream->handle;

    return fread(buffer, 1, bufferSize, handle);
}

sal_callback size_t CPFileReadStreamReadSection(sal_inout CPReadStreamRef baseStream, const size_t offset, sal_out_bcount_opt(bufferSize) byte* buffer, const size_t bufferSize)
{
    CPFileReadStreamRef stream = (CPFileReadStreamRef)baseStream;
    FILE* handle = (FILE*)stream->handle;

    size_t pos = ftello(handle);
    fseeko(handle, offset, SEEK_SET);
    size_t read = fread(buffer, 1, bufferSize, handle);
    fseeko(handle, pos, SEEK_SET);
    return read;
}

void _CPFileReadStreamSetFunctions(sal_inout CPReadStreamRef stream)
{
    stream->close           = CPFileReadStreamClose;
    stream->getLength       = CPFileReadStreamGetLength;
    stream->isEOF           = CPFileReadStreamIsEOF;
    stream->getPosition     = CPFileReadStreamGetPosition;
    stream->seek            = CPFileReadStreamSeek;
    stream->readBuffer      = CPFileReadStreamReadBuffer;
    stream->readSection     = CPFileReadStreamReadSection;
}
