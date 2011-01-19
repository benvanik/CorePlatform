/*
 *  test_url.c
 *  CPUnitTests-OSX
 *
 *  Created by Ben Vanik on 1/18/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "shared.h"

static int test_url_init()
{
    return 0;
}

static int test_url_clean()
{
    return 0;
}

void test_url_create_abs()
{
    CPURLRef url;
    CPStringRef sourceString;
    CPChar buffer[4096];

    const CPChar* validURLs[] = {
        CPTEXT("http://test"),
        CPTEXT("http://test/"),
        CPTEXT("http://test.com"),
        CPTEXT("http://test.com/"),
        CPTEXT("http://test.com/x"),
        CPTEXT("http://test.com/x/"),
        CPTEXT("http://test.com/x."),
        CPTEXT("http://test.com/x.y"),
        CPTEXT("http://test.com/x.y/"),
        CPTEXT("http://test.com/x.y/a"),
        CPTEXT("http://test.com/x.y/a/"),
        CPTEXT("http://test.com/x.y/a."),
        CPTEXT("http://test.com/x.y/a.b"),
        CPTEXT("http://test.com/a%20b/"),
        CPTEXT("http://test.com/a%20%2E"),
        CPTEXT("http://test.com/a?key1=value1&key2=value2"),
        CPTEXT("http://test.com/a/?key1=value1&key2=value2"),
        CPTEXT("http://test.com/a#fragment"),
        CPTEXT("http://test.com/a/#fragment"),
        CPTEXT("http://test.com/a?key1=value1&key2=value2#fragment"),
        CPTEXT("http://test.com/a/?key1=value1&key2=value2#fragment"),
        CPTEXT("http:///"),
        CPTEXT("http:///x"),
        CPTEXT("http://test:123"),
        CPTEXT("http://test:123/"),
        CPTEXT("http://test.com:123"),
        CPTEXT("http://test.com:123/"),
        CPTEXT("http://test.com:123/x"),
        CPTEXT("http://test.com:123/x/"),
    };

    const CPChar* invalidURLs[] = {
        CPTEXT(""),
        CPTEXT("h"),
        CPTEXT("http:"),
        CPTEXT("http:/"),
        CPTEXT("http://"),
        CPTEXT("http://test:"),
        CPTEXT("http://test:/"),
        CPTEXT("http://test:XXX"),
        CPTEXT("http://test:XXX/"),
    };

    for (size_t n = 0; n < CPCOUNT(validURLs); n++) {
        const CPChar* source = validURLs[n];
        url = CPURLCreate(NULL, source);
        CU_ASSERT(url != NULL);
        if (!url) {
            int x = 123; // for debugging
        }
        CU_ASSERT_TRUE(CPURLGetAbsoluteString(url, buffer, sizeof(buffer)));
        CU_ASSERT(CPStrCmp(source, buffer) == 0);
        CPRelease(url);

        sourceString = CPStringCreate(source);
        CU_ASSERT(sourceString != NULL);
        url = CPURLCreateWithString(NULL, sourceString);
        CU_ASSERT(url != NULL);
        CU_ASSERT_TRUE(CPURLGetAbsoluteString(url, buffer, sizeof(buffer)));
        CU_ASSERT(CPStrCmp(source, buffer) == 0);
        CPRelease(url);
        CPRelease(sourceString);
    }

    for (size_t n = 0; n < CPCOUNT(invalidURLs); n++) {
        const CPChar* source = invalidURLs[n];
        url = CPURLCreate(NULL, source);
        CU_ASSERT(url == NULL);
        if (url) {
            int x = 123; // for debugging
        }

        sourceString = CPStringCreate(source);
        CU_ASSERT(sourceString != NULL);
        url = CPURLCreateWithString(NULL, sourceString);
        CU_ASSERT(url == NULL);
        CPRelease(url);
        CPRelease(sourceString);
    }
}

void test_url_create_rel()
{
    CPURLRef url;
    
}

void test_url_create_copy()
{
    CPURLRef url;
    
}

void test_url_gets()
{
    CPURLRef url;
    
}

static CU_TestInfo test_url_infos[] = {
    { "test_url_create_abs",            test_url_create_abs             },
    { "test_url_create_rel",            test_url_create_rel             },
    { "test_url_create_copy",           test_url_create_copy            },
    { "test_url_gets",                  test_url_gets                   },
};

CU_REGISTER_SUITE(test_url);
