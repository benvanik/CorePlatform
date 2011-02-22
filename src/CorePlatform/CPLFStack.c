/*
 *  CPLFStack.c
 *  CorePlatform
 *
 *  Created by Ben Vanik on 2/20/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPLFStack.h>

CP_DEFINE_TYPE(CPLFStack, NULL, CPLFStackDealloc);

CP_API sal_checkReturn sal_out_opt CPLFStackRef CPLFStackCreate(const BOOL opaquePointers, const size_t pointerOffset)
{
    CPASSERT((pointerOffset % CP_ALIGNMENT) == 0);

    CPLFStackRef stack = (CPLFStackRef)CPObjectAlloc(&CPLFStackType, sizeof(CPLFStack));
    CPEXPECTNOTNULL(stack);

    stack->opaquePointers   = opaquePointers;
    stack->pointerOffset    = pointerOffset;

    CPAtomicStackSetup(&stack->inner);

    return stack;

CPCLEANUP:
    CPRelease(stack);
    return NULL;
}

sal_callback void CPLFStackDealloc(sal_inout CPLFStackRef stack)
{
    if (stack->opaquePointers) {
        // Bad case - leaking items here!
        // TODO: warn unpopped items in stack?
    } else {
        void *value;
        while (value = CPLFStackPop(stack)) {
            CPRelease(value);
        }
    }
}

CP_API void CPLFStackPush(sal_inout CPLFStackRef stack, sal_inout void* value)
{
    CPASSERT(((uint64)value) % CP_ALIGNMENT == 0);

    if (!stack->opaquePointers) {
        CPIGNORE(CPRetain(value));
    }

    CPAtomicStackEnqueue(&stack->inner, value, stack->pointerOffset);
}

CP_API sal_out_opt void* CPLFStackPop(sal_inout CPLFStackRef stack)
{
    void* value = CPAtomicStackDequeue(&stack->inner, stack->pointerOffset);
    if (!value) {
        return NULL;
    }

    // NULL out pointer for safety
    *((void**)(((byte*)value) + stack->pointerOffset)) = NULL;

    return value;
}
