/*
 *  CPStr.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 7/19/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPStr.h>

#if !CP_LIKE(WIN32)

// TODO: the semantics of this are still likely wrong w.r.t. failure cases
int strncpy_s(char* dest, size_t destLength, const char* source, size_t count)
{
    size_t length = destLength < count ? destLength : count;
    strncpy(dest, source, length);
    dest[length] = 0;
    return 0;
}

#endif // WIN32

