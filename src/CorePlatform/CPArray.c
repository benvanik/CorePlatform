/*
 *  CPArray.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/17/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPArray.h>

CP_DEFINE_TYPE(CPArray, NULL, CPArrayDealloc);

CP_API sal_checkReturn sal_out_opt CPArrayRef CPArrayCreate(const size_t initialCapacity)
{
    CPArrayRef array = (CPArrayRef)CPObjectAlloc(&CPArrayType, sizeof(CPArray));
    CPEXPECTNOTNULL(array);
    
    array->capacity = initialCapacity;
    array->count    = 0;
    
    size_t itemsSize;
    CPEXPECTTRUE(CPMultSizeT(array->capacity, sizeof(cpptr), &itemsSize));
    
    array->items    = (cpptr*)CPAlloc(itemsSize);
    CPEXPECTNOTNULL(array->items);
    
    return array;
    
CPCLEANUP:
    CPRelease(array);
    return NULL;
}

sal_callback void CPArrayDealloc(sal_inout CPArrayRef array)
{
    CPArrayRemoveAllItems(array);
    CPFree(array->items);
}

CP_API size_t CPArrayGetCapacity(sal_inout CPArrayRef array)
{
    return array->capacity;
}

CP_API size_t CPArrayGetCount(sal_inout CPArrayRef array)
{
    return array->count;
}

CP_API BOOL CPArrayExpand(sal_inout CPArrayRef array, const size_t newCapacity)
{
    if (array->capacity >= newCapacity) {
        return TRUE;
    }
    
    size_t oldSize;
    CPEXPECTTRUE(CPMultSizeT(array->capacity, sizeof(cpptr), &oldSize));
    size_t newSize;
    CPEXPECTTRUE(CPMultSizeT(newCapacity, sizeof(cpptr), &newSize));
    
    cpptr* newItems = (cpptr*)CPRealloc(array->items, oldSize, newSize);
    CPEXPECTNOTNULL(newItems);
    
    array->capacity = newCapacity;
    array->items    = newItems;
    
    return TRUE;
    
CPCLEANUP:
    return FALSE;
}

CP_API sal_out_opt cpptr CPArrayGetItem(sal_inout CPArrayRef array, const size_t index)
{
    if (index > array->count) {
        return NULL;
    }
    return array->items[index];
}

CP_API size_t CPArrayGetIndexOfItem(sal_inout CPArrayRef array, sal_inout cpptr item)
{
    if (!item) {
        return CPArrayInvalidIndex;
    }
    for (size_t n = 0; n < array->count; n++) {
        if (array->items[n] == item) {
            return n;
        }
    }
    return CPArrayInvalidIndex;
}

CP_API BOOL CPArrayAddItem(sal_inout CPArrayRef array, sal_inout cpptr item)
{
    if (!item) {
        return FALSE;
    }
    
    size_t newCount;
    if (!CPAddSizeT(array->count, 1, &newCount)) {
        return FALSE;
    }
    
    if (newCount > array->capacity) {
        size_t newCapacity = array->capacity;
        size_t half = newCapacity / 2;
        half = MAX(64, half);
        newCapacity += half;
        if (!CPArrayExpand(array, newCapacity)) {
            return FALSE;
        }
    }
    
    const size_t index = array->count;
    array->count = newCount;
    
    array->items[index] = CPRetain(item);
    
    return TRUE;
}

CP_API BOOL CPArrayRemoveItem(sal_inout CPArrayRef array, sal_inout cpptr item)
{
    size_t index = CPArrayGetIndexOfItem(array, item);
    if (index == CPArrayInvalidIndex) {
        return FALSE;
    }
    return CPArrayRemoveItemAtIndex(array, index);
}

CP_API BOOL CPArrayRemoveItemAtIndex(sal_inout CPArrayRef array, const size_t index)
{
    if (index > array->count) {
        return FALSE;
    }
    
    cpptr item = array->items[index];
    
    // Shift all items down via memcpy
    void* slot = &array->items[index];
    size_t length;
    CPEXPECTTRUE(CPMultSizeT(array->count - index, sizeof(cpptr), &length));
    CPEXPECTTRUE(CPCopyMemory(slot, length, (byte*)slot + sizeof(cpptr), length));
    
    array->count--;
    CPRelease(item);
    
    return TRUE;
    
CPCLEANUP:
    return FALSE;
}

CP_API void CPArrayRemoveAllItems(sal_inout CPArrayRef array)
{
    for (size_t n = 0; n < array->count; n++) {
        cpptr item = array->items[n];
        CPRelease(item);
    }
    array->count = 0;
}
