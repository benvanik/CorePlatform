/*
 *  CPConfig.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CPPlatform.h>

#define CP_OPTION(NAME)     (defined CP_OPTION_##NAME   && CP_OPTION_##NAME     )

#define CP_OPTION_ENABLE_ASSERTS        1

#define CP_OPTION_ENABLE_LOGGING        1
#define CP_OPTION_LOG_ERROR             1
#define CP_OPTION_LOG_WARNING           1
#define CP_OPTION_LOG_INFO              1
#define CP_OPTION_LOG_DEBUG             1
