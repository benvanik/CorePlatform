/*
 *  CPCommon.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <CorePlatform/CPPlatform.h>
#include <CorePlatform/CPConfig.h>
#include <CorePlatform/CPTypes.h>
#include <CorePlatform/CPAnnotations.h>
#include <CorePlatform/CPCodeUtilities.h>
#include <CorePlatform/CPMath.h>
#include <CorePlatform/CPAtomic.h>
#include <CorePlatform/CPMemory.h>
#include <CorePlatform/CPDebugging.h>
#include <CorePlatform/CPStr.h>
#include <CorePlatform/CPObject.h>
#include <CorePlatform/CPArray.h>
#include <CorePlatform/CPLFStack.h>
#include <CorePlatform/CPLinkedList.h>
#include <CorePlatform/CPString.h>
#include <CorePlatform/CPURL.h>
#include <CorePlatform/CPPAL.h>

// NOTE: these should exist on all platforms
#include <stddef.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <locale.h>
#include <errno.h>

#ifdef __cplusplus
}
#endif
