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

// url | relative path | url + relative
const CPChar* test_url_validURLs[] = {
    CPTEXT("http://test"),                                  CPTEXT("path/file"),            CPTEXT("http://test/path/file"),
    CPTEXT("http://test/"),                                 CPTEXT("path/file"),            CPTEXT("http://test/path/file"),
    CPTEXT("http://test"),                                  CPTEXT("/path/file"),           CPTEXT("http://test/path/file"),
    CPTEXT("http://test/"),                                 CPTEXT("/path/file"),           CPTEXT("http://test/path/file"),
    CPTEXT("http://test.com"),                              CPTEXT("path/file"),            CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/"),                             CPTEXT("path/file"),            CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com"),                              CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/"),                             CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x"),                            CPTEXT("path/file"),            CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x/"),                           CPTEXT("path/file"),            CPTEXT("http://test.com/x/path/file"),
    CPTEXT("http://test.com/x"),                            CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x/"),                           CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x."),                           CPTEXT("path/file"),            CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x./"),                          CPTEXT("path/file"),            CPTEXT("http://test.com/x./path/file"),
    CPTEXT("http://test.com/x."),                           CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x./"),                          CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x.y"),                          CPTEXT("path/file"),            CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x.y/"),                         CPTEXT("path/file"),            CPTEXT("http://test.com/x.y/path/file"),
    CPTEXT("http://test.com/x.y"),                          CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x.y/"),                         CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x.y/a"),                        CPTEXT("path/file"),            CPTEXT("http://test.com/x.y/path/file"),
    CPTEXT("http://test.com/x.y/a/"),                       CPTEXT("path/file"),            CPTEXT("http://test.com/x.y/a/path/file"),
    CPTEXT("http://test.com/x.y/a"),                        CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x.y/a/"),                       CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x.y/a."),                       CPTEXT("path/file"),            CPTEXT("http://test.com/x.y/path/file"),
    CPTEXT("http://test.com/x.y/a.b"),                      CPTEXT("path/file"),            CPTEXT("http://test.com/x.y/path/file"),
    CPTEXT("http://test.com/x.y/a."),                       CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/x.y/a.b"),                      CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/a%20b/"),                       CPTEXT("path/f%20ile"),         CPTEXT("http://test.com/a%20b/path/f%20ile"),
    CPTEXT("http://test.com/a%20%2E"),                      CPTEXT("path/f%20ile"),         CPTEXT("http://test.com/path/f%20ile"),
    CPTEXT("http://test.com/a%20b/"),                       CPTEXT("/path/f%20ile"),        CPTEXT("http://test.com/path/f%20ile"),
    CPTEXT("http://test.com/a%20%2E"),                      CPTEXT("/path/f%20ile"),        CPTEXT("http://test.com/path/f%20ile"),
    CPTEXT("http://test.com/a?k1=v1&k2=v2"),                CPTEXT("path/file"),            CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/a/?k1=v1&k2=v2"),               CPTEXT("path/file"),            CPTEXT("http://test.com/a/path/file"),
    CPTEXT("http://test.com/a?k1=v1&k2=v2"),                CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/a/?k1=v1&k2=v2"),               CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/a#fragment"),                   CPTEXT("path/file"),            CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/a/#fragment"),                  CPTEXT("path/file"),            CPTEXT("http://test.com/a/path/file"),
    CPTEXT("http://test.com/a#fragment"),                   CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/a/#fragment"),                  CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/a/#fragment"),                  CPTEXT("path/#frag"),           CPTEXT("http://test.com/a/path/#frag"),
    CPTEXT("http://test.com/a#fragment"),                   CPTEXT("path/file#frag"),       CPTEXT("http://test.com/path/file#frag"),
    CPTEXT("http://test.com/a/#fragment"),                  CPTEXT("#FRAG"),                CPTEXT("http://test.com/a/#FRAG"),
    CPTEXT("http://test.com/a/#fragment"),                  CPTEXT("/#FRAG"),               CPTEXT("http://test.com/#FRAG"),
    CPTEXT("http://test.com/a?k1=v1&k2=v2#frag"),           CPTEXT("path/file"),            CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/a/?k1=v1&k2=v2#frag"),          CPTEXT("path/file"),            CPTEXT("http://test.com/a/path/file"),
    CPTEXT("http://test.com/a?k1=v1&k2=v2#frag"),           CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/a/?k1=v1&k2=v2#frag"),          CPTEXT("/path/file"),           CPTEXT("http://test.com/path/file"),
    CPTEXT("http://test.com/a?k1=v1&k2=v2#frag"),           CPTEXT("path/file?K=V"),        CPTEXT("http://test.com/path/file?K=V"),
    CPTEXT("http://test.com/a/?k1=v1&k2=v2#frag"),          CPTEXT("path/file?K=V"),        CPTEXT("http://test.com/a/path/file?K=V"),
    CPTEXT("http://test.com/a?k1=v1&k2=v2#frag"),           CPTEXT("/path/file?K=V"),       CPTEXT("http://test.com/path/file?K=V"),
    CPTEXT("http://test.com/a/?k1=v1&k2=v2#frag"),          CPTEXT("/path/file?K=V"),       CPTEXT("http://test.com/path/file?K=V"),
    CPTEXT("http://test.com/a"),                            CPTEXT("?K=V"),                 CPTEXT("http://test.com/a?K=V"),
    CPTEXT("http://test.com/a/"),                           CPTEXT("?K=V"),                 CPTEXT("http://test.com/a/?K=V"),
    CPTEXT("http://test.com/a"),                            CPTEXT("/?K=V"),                CPTEXT("http://test.com/?K=V"),
    CPTEXT("http://test.com/a/"),                           CPTEXT("/?K=V"),                CPTEXT("http://test.com/?K=V"),
    CPTEXT("http:///"),                                     CPTEXT("path/file"),            CPTEXT("http:///path/file"),
    CPTEXT("http:///x"),                                    CPTEXT("path/file"),            CPTEXT("http:///path/file"),
    CPTEXT("http:///"),                                     CPTEXT("/path/file"),           CPTEXT("http:///path/file"),
    CPTEXT("http:///x"),                                    CPTEXT("/path/file"),           CPTEXT("http:///path/file"),
    CPTEXT("http://test:123"),                              CPTEXT("path/file"),            CPTEXT("http://test:123/path/file"),
    CPTEXT("http://test:123/"),                             CPTEXT("path/file"),            CPTEXT("http://test:123/path/file"),
    CPTEXT("http://test:123"),                              CPTEXT("/path/file"),           CPTEXT("http://test:123/path/file"),
    CPTEXT("http://test:123/"),                             CPTEXT("/path/file"),           CPTEXT("http://test:123/path/file"),
    CPTEXT("http://test.com:123"),                          CPTEXT("path/file"),            CPTEXT("http://test.com:123/path/file"),
    CPTEXT("http://test.com:123/"),                         CPTEXT("path/file"),            CPTEXT("http://test.com:123/path/file"),
    CPTEXT("http://test.com:123/x"),                        CPTEXT("/path/file"),           CPTEXT("http://test.com:123/path/file"),
    CPTEXT("http://test.com:123/x/"),                       CPTEXT("/path/file"),           CPTEXT("http://test.com:123/path/file"),
    CPTEXT("about:blank"),                                  CPTEXT("path/file"),            CPTEXT("about:blank/path/file"),
    CPTEXT("about:blank/"),                                 CPTEXT("path/file"),            CPTEXT("about:blank/path/file"),
    CPTEXT("about:blank/test1"),                            CPTEXT("path/file"),            CPTEXT("about:blank/path/file"),
    CPTEXT("about:blank/test1/"),                           CPTEXT("path/file"),            CPTEXT("about:blank/test1/path/file"),
    CPTEXT("about:blank"),                                  CPTEXT("/path/file"),           CPTEXT("about:blank/path/file"),
    CPTEXT("about:blank/"),                                 CPTEXT("/path/file"),           CPTEXT("about:blank/path/file"),
    CPTEXT("about:blank/test1"),                            CPTEXT("/path/file"),           CPTEXT("about:blank/path/file"),
    CPTEXT("about:blank/test1/"),                           CPTEXT("/path/file"),           CPTEXT("about:blank/path/file"),
};

void test_url_create_abs()
{
    CPURLRef url;
    CPStringRef sourceString;
    CPChar buffer[4096];

    const CPChar* invalidURLs[] = {
        CPTEXT(""),
        CPTEXT("h"),
        CPTEXT("http:"),
        CPTEXT("http:/"),
        CPTEXT("http://"),
        CPTEXT("http://test:"),
        CPTEXT("http://test:/"),
    };

    for (size_t n = 0; n < CPCOUNT(test_url_validURLs); n += 3) {
        const CPChar* source = test_url_validURLs[n + 0];
        const CPChar* verify = test_url_validURLs[n + 0];

        url = CPURLCreate(NULL, source);
        CU_ASSERT(url != NULL);
        if (!url) {
            int x = 123; // for debugging
        }
        CPZeroMemory(buffer, sizeof(buffer), 0, sizeof(buffer));
        CU_ASSERT_TRUE(CPURLGetAbsoluteString(url, buffer, sizeof(buffer)));
        CU_ASSERT(CPStrCmp(verify, buffer) == 0);
        if (CPStrCmp(verify, buffer) != 0) {
            int x = 123; // for debugging
        }
        CPRelease(url);

        sourceString = CPStringCreate(source);
        CU_ASSERT(sourceString != NULL);
        url = CPURLCreateWithString(NULL, sourceString);
        CU_ASSERT(url != NULL);
        CPZeroMemory(buffer, sizeof(buffer), 0, sizeof(buffer));
        CU_ASSERT_TRUE(CPURLGetAbsoluteString(url, buffer, sizeof(buffer)));
        CU_ASSERT(CPStrCmp(verify, buffer) == 0);
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
    CPURLRef sub;
    CPChar buffer[4096];

    /*const CPChar* invalidURLs[] = {
        CPTEXT(""),
        CPTEXT("h"),
        CPTEXT("http:"),
        CPTEXT("http:/"),
        CPTEXT("http://"),
        CPTEXT("http://test:"),
        CPTEXT("http://test:/"),
    };*/

    for (size_t n = 0; n < CPCOUNT(test_url_validURLs); n += 3) {
        const CPChar* sourceBase    = test_url_validURLs[n + 0];
        const CPChar* sourceSub     = test_url_validURLs[n + 1];
        const CPChar* sourceVerify  = test_url_validURLs[n + 2];

        url = CPURLCreate(NULL, sourceBase);
        CU_ASSERT(url != NULL);
        sub = CPURLCreate(url, sourceSub);
        CU_ASSERT(sub != NULL);
        if (!sub) {
            int x = 123; // for debugging
        }
        CPZeroMemory(buffer, sizeof(buffer), 0, sizeof(buffer));
        CU_ASSERT_TRUE(CPURLGetAbsoluteString(sub, buffer, sizeof(buffer)));
        CU_ASSERT(CPStrCmp(sourceVerify, buffer) == 0);
        if (CPStrCmp(sourceVerify, buffer) != 0) {
            int y = 123;
        }
        CPRelease(sub);
        CPRelease(url);
    }

    /*for (size_t n = 0; n < CPCOUNT(invalidURLs); n += 3) {
        const CPChar* sourceBase    = invalidURLs[n + 0];
        const CPChar* sourceSub     = invalidURLs[n + 1];
        const CPChar* sourceVerify  = invalidURLs[n + 2];

        url = CPURLCreate(NULL, sourceBase);
        CU_ASSERT(url == NULL);
        sub = CPURLCreate(url, sourceSub);
        CU_ASSERT(sub == NULL);
    }*/
}

void test_url_create_copy()
{
    CPURLRef url;
    CPURLRef sub;
    CPURLRef copy;
    CPChar buffer[4096];
    
    for (size_t n = 0; n < CPCOUNT(test_url_validURLs); n += 3) {
        const CPChar* source = test_url_validURLs[n + 0];
        const CPChar* verify = test_url_validURLs[n + 0];

        url = CPURLCreate(NULL, source);
        CU_ASSERT(url != NULL);
        copy = CPURLCreateAbsoluteCopy(url);
        CU_ASSERT(copy != NULL);
        CPZeroMemory(buffer, sizeof(buffer), 0, sizeof(buffer));
        CU_ASSERT_TRUE(CPURLGetAbsoluteString(copy, buffer, sizeof(buffer)));
        CU_ASSERT(CPStrCmp(verify, buffer) == 0);
        if (CPStrCmp(verify, buffer) != 0) {
            int y = 123;
        }
        CPRelease(copy);
        CPRelease(url);
    }

    for (size_t n = 0; n < CPCOUNT(test_url_validURLs); n += 3) {
        const CPChar* sourceBase    = test_url_validURLs[n + 0];
        const CPChar* sourceSub     = test_url_validURLs[n + 1];
        const CPChar* sourceVerify  = test_url_validURLs[n + 2];

        url = CPURLCreate(NULL, sourceBase);
        CU_ASSERT(url != NULL);
        sub = CPURLCreate(url, sourceSub);
        CU_ASSERT(sub != NULL);
        copy = CPURLCreateAbsoluteCopy(sub);
        CU_ASSERT(copy != NULL);
        CPZeroMemory(buffer, sizeof(buffer), 0, sizeof(buffer));
        CU_ASSERT_TRUE(CPURLGetAbsoluteString(copy, buffer, sizeof(buffer)));
        CU_ASSERT(CPStrCmp(sourceVerify, buffer) == 0);
        if (CPStrCmp(sourceVerify, buffer) != 0) {
            int y = 123;
        }
        CPRelease(copy);
        CPRelease(sub);
        CPRelease(url);
    }
}

void test_url_create_large()
{
    CPURLRef url;
    CPURLRef base;

    const size_t largeLength = 16 * 1024;
    CPChar* large = (CPChar*)CPAlloc(largeLength * sizeof(CPChar));
    CPStrCat(large, largeLength, CPTEXT("http://hello.com"));
    for (size_t n = 0; n < largeLength - 16 - 1; n++) {
        if ((n % 10) == 0) {
            large[n + 16] = '/';
        } else {
            large[n + 16] = '0' + (n % 10);
        }
    }
    const size_t largeSubLength = 16 * 1024;
    CPChar* largeSub = (CPChar*)CPAlloc(largeSubLength * sizeof(CPChar));
    for (size_t n = 0; n < largeSubLength - 1; n++) {
        if ((n % 10) == 0) {
            largeSub[n] = '/';
        } else {
            largeSub[n] = '0' + (n % 10);
        }
    }

    url = CPURLCreate(NULL, large);
    CU_ASSERT(url != NULL);
    CPRelease(url);
    
    base = CPURLCreate(NULL, CPTEXT("http://baseurl/"));
    CU_ASSERT(base != NULL);
    url = CPURLCreate(base, largeSub);
    CU_ASSERT(url != NULL);
    CPRelease(url);
    CPRelease(base);

    CPFree(largeSub);
    CPFree(large);
}

void test_url_escape()
{
    // TODO: test CPURLEscape
    CU_FAIL("test_url_escape unimplemented");
}

void test_url_unescape()
{
    // TODO: test CPURLUnescape
    //CU_FAIL("test_url_unescape unimplemented");
}

static CU_TestInfo test_url_infos[] = {
    { "test_url_create_abs",            test_url_create_abs             },
    { "test_url_create_rel",            test_url_create_rel             },
    { "test_url_create_copy",           test_url_create_copy            },
    { "test_url_create_large",          test_url_create_large           },
    { "test_url_escape",                test_url_escape                 },
    { "test_url_unescape",              test_url_unescape               },
};

CU_REGISTER_SUITE(test_url);
