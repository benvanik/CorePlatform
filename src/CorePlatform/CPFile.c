/*
 *  CPFile.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 5/07/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPFile.h>

// TODO: put all FILE methods on the PAL
#if CP_LIKE(WIN32)
#define fseeko                          _fseeki64
#define ftello                          _ftelli64
#else
#endif

CP_DEFINE_TYPE(CPFile, NULL, CPFileDealloc);

sal_checkReturn sal_out_opt CPFileRef _CPFileCreateCore(sal_inout CPPALRef pal, sal_inout CPURLRef path, const CPFileMode mode, const CPChar* modeString)
{
    CPFileRef file = (CPFileRef)CPObjectAlloc(&CPFileType, sizeof(CPFile));
    CPEXPECTNOTNULL(file);
    
    CPChar fullPath[CP_MAX_PATH];
    CPEXPECTTRUE(CPPALConvertURLToFileSystemPath(pal, path, fullPath, CPCOUNT(fullPath)));

    file->path      = CPURLRetain(path);
    file->mode      = mode;
    file->handle    = NULL;

#if CP_LIKE(WIN32)
    CPEXPECTZERO(_wfopen_s((FILE**)&file->handle, fullPath, modeString));
#else
    file->handle = fopen(fullPath, modeString);
    CPEXPECTNOTNULL(file->handle);
#endif

    return file;

CPCLEANUP:
    CPRelease(file);
    return NULL;
}

CP_API sal_checkReturn sal_out_opt CPFileRef CPFileCreate(sal_inout CPPALRef pal, sal_inout CPURLRef path)
{
    return _CPFileCreateCore(pal, path, CPFileModeWrite, CPTEXT("wb"));
}

CP_API sal_checkReturn sal_out_opt CPFileRef CPFileOpen(sal_inout CPPALRef pal, sal_inout CPURLRef path, const CPFileMode mode)
{
    CPChar modeString[10];
    modeString[0] = 0;
    if (mode == CPFileModeRead) {
        CPStrCat(modeString, CPCOUNT(modeString), CPTEXT("r"));
    } else if (mode == CPFileModeWrite) {
        CPStrCat(modeString, CPCOUNT(modeString), CPTEXT("w"));
    }
    CPStrCat(modeString, CPCOUNT(modeString), CPTEXT("b"));

    return _CPFileCreateCore(pal, path, mode, modeString);
}

sal_callback void CPFileDealloc(sal_inout CPFileRef file)
{
    CPFileClose(file);
    CPRelease(file->path);
}

CP_API void CPFileClose(sal_inout CPFileRef file)
{
    if (!file->handle) {
        return;
    }

    FILE* handle = (FILE*)file->handle;
    fclose(handle);

    file->handle = NULL;
}

CP_API CPURLRef CPFileGetPath(sal_inout CPFileRef file)
{
    return file->path;
}

CP_API size_t CPFileGetSize(sal_inout CPFileRef file)
{
    if (!file->handle) {
        return CPInvalidSize;
    }

    FILE* handle = (FILE*)file->handle;

    fseeko(handle, 0, SEEK_END);
    return ftello(handle);
}

//CP_API BOOL CPFileSetSize(sal_inout CPFileRef file, const size_t newSize)
//{
//    if (!file->handle) {
//        return CPFileInvalidSize;
//    }
//
//    FILE* handle = (FILE*)file->handle;
//
//    // TODO: expand file
//    return FALSE;
//}

CP_API size_t CPFileRead(sal_inout CPFileRef file, const size_t offset, sal_out_bcount_opt(bufferSize) byte* buffer, const size_t bufferSize)
{
    if (!file->handle) {
        return CPInvalidSize;
    }

    FILE* handle = (FILE*)file->handle;

    if (fseeko(handle, offset, SEEK_SET) != 0) {
        return CPInvalidSize;
    }

    const size_t read = fread(buffer, 1, bufferSize, handle);
    return read;
}

CP_API BOOL CPFileWrite(sal_inout CPFileRef file, const size_t offset, sal_in_bcount(bufferSize) const byte* buffer, const size_t bufferSize)
{
    if (!file->handle) {
        return FALSE;
    }

    FILE* handle = (FILE*)file->handle;

    const size_t written = fwrite(buffer, 1, bufferSize, handle);
    return written == bufferSize;
}
