/*
 *  CPFileReadStream.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 5/08/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CorePlatform/CPURL.h>
#include <CorePlatform/CPPAL.h>
#include <CorePlatform/CPReadStream.h>

CP_EXTERNC_BEGIN

CP_DECLARE_TYPE(CPFileReadStream);

struct CPFileReadStream_t {
    CPReadStream    base;
    
    CPPALRef        pal;
    CPURLRef        path;

    void*           handle;
};

CP_API sal_checkReturn sal_out_opt CPFileReadStreamRef CPFileReadStreamCreateWithPath(sal_inout CPPALRef pal, sal_inout CPURLRef path);

CP_EXTERNC_END
