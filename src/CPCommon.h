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

#include <CPPlatform.h>
#include <CPConfig.h>
#include <CPTypes.h>
#include <CPAnnotations.h>
#include <CPCodeUtilities.h>
#include <CPMath.h>
#include <CPAtomic.h>
#include <CPMemory.h>
#include <CPDebugging.h>
#include <CPStr.h>
#include <CPObject.h>
#include <CPArray.h>
#include <CPLinkedList.h>
#include <CPString.h>
#include <CPURL.h>
#include <CPPAL.h>

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
