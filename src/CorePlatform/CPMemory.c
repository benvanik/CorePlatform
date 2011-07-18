/*
 *  CPMemory.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPMemory.h>
#include <CorePlatform/CPMath.h>

#include <stdlib.h>

#define CPALLOCSAFE(size)   calloc(1, size)
#define CPREALLOCSAFE       realloc
#define CPFREESAFE          free

CP_API sal_checkReturn sal_out_bcount_opt(size) void* CPAlloc(const size_t size)
{
    // Zero size always returns a valid value
    if (!size) {
        return CPALLOCSAFE(8);
    }
    
    return CPALLOCSAFE(size);
}

CP_API sal_checkReturn sal_out_bcount_opt(newSize) void* CPRealloc(sal_inout_bcount_opt(oldSize) void* ptr, const size_t oldSize, const size_t newSize)
{
    // Handle using realloc as malloc
    if (!ptr) {
        return CPAlloc(newSize);
    }
    
    // Handle no-op
    if (oldSize == newSize) {
        return ptr;
    }
    
    // Support zero-size reallocs by allocating a dummy buffer and returning that
    if (!newSize) {
        void* dummy = CPALLOCSAFE(4);
        if (!dummy) {
            return NULL;
        }
        CPFree(ptr);
        return dummy;
    }
    
    void* result = CPREALLOCSAFE(ptr, newSize);
    if (result) {
        // Zero out any new memory
        if (newSize > oldSize) {
            const size_t offset = oldSize;
            size_t length;
            if (CPSubSizeT(newSize, oldSize, &length)) {
                CPZeroMemory(result, newSize, offset, length);
            } else {
                // Failed to zero memory - fail out (so caller doesn't try to use the uninitialized memory)
                return NULL;
            }
        }
    }
    return result;
}

CP_API void _CPFreeCore(sal_inout_opt void* ptr)
{
    CPFREESAFE(ptr);
}

CP_API sal_checkReturn sal_out_bcount_opt(size * count) void* CPAllocArray(const size_t count, const size_t size)
{
    size_t bytes;
    if (!CPMultSizeT(count, size, &bytes)) {
        return NULL;
    }
    return CPAlloc(bytes);
}

CP_API void _CPFreeArray(sal_inout_opt void* ptr)
{
    _CPFreeCore(ptr);
}

CP_API sal_checkReturn sal_out_bcount_opt(size) CPAlignedVoid* CPAllocAligned(const size_t size)
{
    // TODO: validate platform alignment to CP_ALIGNMENT
    return CPAlloc(size);
}

CP_API void _CPFreeAligned(sal_inout_opt CPAlignedVoid* ptr)
{
    _CPFreeCore((void*)ptr);
}

CP_API BOOL CPZeroMemory(sal_inout_bcount(size) void* ptr, const size_t size, const size_t offset, const size_t length)
{
    // TODO: validate sizing is right/clamp to size
    if (!ptr || !length) {
        return TRUE;
    }
    size_t end;
    if (!CPAddSizeT(offset, length, &end)) {
        return FALSE;
    }
    if (end > size) {
        return FALSE;
    }
    
    memset((byte*)ptr + offset, 0, length);
    return TRUE;
}

CP_API BOOL CPCopyMemory(sal_out_bcount(destSize) void* dest, const size_t destSize, sal_in_bcount(sourceSize) const void* source, const size_t sourceSize)
{
    // TODO: verify sizing is right
    if (!sourceSize) {
        return TRUE;
    }
    if (!dest || !source) {
        return FALSE;
    }
    if (destSize < sourceSize) {
        return FALSE;
    }
    
    memcpy(dest, source, sourceSize);
    return TRUE;
}

#if !CP_LIKE(WIN32)
#include <sys/mman.h>
#endif

CP_API sal_checkReturn sal_out_bcount_opt(size) void* CPVirtualAlloc(sal_inout_bcount(size) void* ptr, const size_t size)
{
#if CP_LIKE(WIN32)
    return VirtualAlloc(ptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
    return mmap(ptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED, 0, 0);
#endif
}

CP_API void CPVirtualFree(sal_inout_bcount(size) void* ptr, const size_t size)
{
#if CP_LIKE(WIN32)
    VirtualFree(ptr, size, MEM_RELEASE);
#else
    munmap(ptr, size);
#endif
}

