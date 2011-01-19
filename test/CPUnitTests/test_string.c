/*
 *  test_string.c
 *  CPUnitTests-OSX
 *
 *  Created by Ben Vanik on 1/18/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include "shared.h"

static int test_string_init()
{
    return 0;
}

static int test_string_clean()
{
    return 0;
}

void test_string_create()
{
    CPStringRef string;
    const CPChar* value;
    
    string = CPStringCreate(CPTEXT(""));
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 0);
    CU_ASSERT(CPStrCmp(value, CPTEXT("")) == 0);
    CPRelease(string);
    
    string = CPStringCreate(CPTEXT("a"));
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 1);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 1);
    CU_ASSERT(CPStrCmp(value, CPTEXT("a")) == 0);
    CPRelease(string);
    
    string = CPStringCreate(CPTEXT("12345678901234567890"));
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 20);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 20);
    CU_ASSERT(CPStrCmp(value, CPTEXT("12345678901234567890")) == 0);
    CPRelease(string);

    string = CPStringCreateWithCharacters(CPTEXT("12345678901234567890"), 0);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 0);
    CU_ASSERT(CPStrCmp(value, CPTEXT("")) == 0);
    CPRelease(string);

    string = CPStringCreateWithCharacters(CPTEXT("12345678901234567890"), 10);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 10);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 10);
    CU_ASSERT(CPStrCmp(value, CPTEXT("1234567890")) == 0);
    CPRelease(string);
}

void test_string_createsubstring()
{
    CPStringRef base;
    CPStringRef string;
    const CPChar* value;

    base = CPStringCreate(CPTEXT("12345678901234567890"));
    CU_ASSERT(base != NULL);

    string = CPStringCreateWithSubstring(base, 0, 20);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 20);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 20);
    CU_ASSERT(CPStrCmp(value, CPTEXT("12345678901234567890")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 0, 10);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 10);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 10);
    CU_ASSERT(CPStrCmp(value, CPTEXT("1234567890")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 10, 10);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 10);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 10);
    CU_ASSERT(CPStrCmp(value, CPTEXT("1234567890")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 0, 1);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 1);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 1);
    CU_ASSERT(CPStrCmp(value, CPTEXT("1")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 10, 1);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 1);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 1);
    CU_ASSERT(CPStrCmp(value, CPTEXT("1")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 19, 1);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 1);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 1);
    CU_ASSERT(CPStrCmp(value, CPTEXT("0")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 18, 2);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 2);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 2);
    CU_ASSERT(CPStrCmp(value, CPTEXT("90")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 10, 10);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 10);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 10);
    CU_ASSERT(CPStrCmp(value, CPTEXT("1234567890")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 0, 0);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 0);
    CU_ASSERT(CPStrCmp(value, CPTEXT("")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 1, 0);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 0);
    CU_ASSERT(CPStrCmp(value, CPTEXT("")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 10, 20);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 10);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 10);
    CU_ASSERT(CPStrCmp(value, CPTEXT("1234567890")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 19, 10);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 1);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 1);
    CU_ASSERT(CPStrCmp(value, CPTEXT("0")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 20, 1);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 0);
    CU_ASSERT(CPStrCmp(value, CPTEXT("")) == 0);
    CPRelease(string);

    string = CPStringCreateWithSubstring(base, 21, 1);
    CU_ASSERT(string == NULL);

    CPRelease(base);
}

void test_string_createformat()
{
    CPStringRef string;
    const CPChar* value;

    const size_t massiveLength = 16 * 1024;
    CPChar* massiveString = (CPChar*)CPAlloc((massiveLength + 1) * sizeof(CPChar));
    CU_ASSERT(massiveString != NULL);
    for (size_t n = 0; n < massiveLength; n++) {
        massiveString[n] = '0' + (n % 10);
    }
    const size_t massiveFormatLength = 16 * 1024 - 4;
    CPChar* massiveFormatString = (CPChar*)CPAlloc((massiveFormatLength + 2 + 1) * sizeof(CPChar));
    CU_ASSERT(massiveFormatString != NULL);
    CPChar* massiveFormatValidateString = (CPChar*)CPAlloc((massiveFormatLength + 4 + 1) * sizeof(CPChar));
    CU_ASSERT(massiveFormatValidateString != NULL);
    massiveFormatString[0] = '%';
    massiveFormatString[1] = 'd';
    massiveFormatValidateString[0] = '1';
    massiveFormatValidateString[1] = '2';
    massiveFormatValidateString[2] = '3';
    massiveFormatValidateString[3] = '4';
    for (size_t n = 0; n < massiveFormatLength; n++) {
        massiveFormatString[n + 2] = '0' + (n % 10);
        massiveFormatValidateString[n + 4] = '0' + (n % 10);
    }

    string = CPStringCreateWithFormat(CPTEXT(""));
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 0);
    CU_ASSERT(CPStrCmp(value, CPTEXT("")) == 0);
    CPRelease(string);

    string = CPStringCreateWithFormat(CPTEXT("a"));
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 1);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 1);
    CU_ASSERT(CPStrCmp(value, CPTEXT("a")) == 0);
    CPRelease(string);

    string = CPStringCreateWithFormat(massiveString);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == massiveLength);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == massiveLength);
    CU_ASSERT(CPStrCmp(value, massiveString) == 0);
    CPRelease(string);

    string = CPStringCreateWithFormat(CPTEXT("a%db"), 1234);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 6);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 6);
    CU_ASSERT(CPStrCmp(value, CPTEXT("a1234b")) == 0);
    CPRelease(string);

    string = CPStringCreateWithFormat(CPTEXT("a%sb"), CPTEXT("FOOD"));
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 6);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 6);
    CU_ASSERT(CPStrCmp(value, CPTEXT("aFOODb")) == 0);
    CPRelease(string);

    string = CPStringCreateWithFormat(CPTEXT("a%gb"), 1.234);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 7);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 7);
    CU_ASSERT(CPStrCmp(value, CPTEXT("a1.234b")) == 0);
    CPRelease(string);

    string = CPStringCreateWithFormat(CPTEXT("a%db%sc%gd"), 1234, CPTEXT("FOOD"), 1.234);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 17);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 17);
    CU_ASSERT(CPStrCmp(value, CPTEXT("a1234bFOODc1.234d")) == 0);
    CPRelease(string);

    // TODO: other formatter types?

    string = CPStringCreateWithFormat(massiveFormatString, 1234);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == massiveFormatLength + 4);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == massiveFormatLength + 4);
    CU_ASSERT(CPStrCmp(value, massiveFormatValidateString) == 0);
    CPRelease(string);

    CPFree(massiveFormatValidateString);
    CPFree(massiveFormatString);
    CPFree(massiveString);
}

void test_string_createcopy()
{
    CPStringRef string1;
    CPStringRef string2;
    const CPChar* value;

    string1 = CPStringCreate(CPTEXT(""));
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(CPStringGetLength(string1) == 0);
    string2 = CPStringCreateCopy(string1);
    CU_ASSERT(string2 != NULL);
    CU_ASSERT(CPStringGetLength(string2) == 0);
    value = CPStringGet(string2);
    CU_ASSERT(CPStrLen(value) == 0);
    CU_ASSERT(CPStrCmp(value, CPTEXT("")) == 0);
    CPRelease(string2);
    CPRelease(string1);
    
    string1 = CPStringCreate(CPTEXT("a"));
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(CPStringGetLength(string1) == 1);
    string2 = CPStringCreateCopy(string1);
    CU_ASSERT(string2 != NULL);
    CU_ASSERT(CPStringGetLength(string2) == 1);
    value = CPStringGet(string2);
    CU_ASSERT(CPStrLen(value) == 1);
    CU_ASSERT(CPStrCmp(value, CPTEXT("a")) == 0);
    CPRelease(string2);
    CPRelease(string1);
    
    string1 = CPStringCreate(CPTEXT("12345678901234567890"));
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(CPStringGetLength(string1) == 20);
    string2 = CPStringCreateCopy(string1);
    CU_ASSERT(string2 != NULL);
    CU_ASSERT(CPStringGetLength(string2) == 20);
    value = CPStringGet(string2);
    CU_ASSERT(CPStrLen(value) == 20);
    CU_ASSERT(CPStrCmp(value, CPTEXT("12345678901234567890")) == 0);
    CPRelease(string2);
    CPRelease(string1);
}

void test_string_createcombining()
{
    CPStringRef string;

    const CPChar* sep0 = CPTEXT("");
    const CPChar* sep1 = CPTEXT(",");
    const CPChar* sep2 = CPTEXT(", ");

    CPArrayRef array0 = CPArrayCreate(0);
    CPArrayRef array1 = CPArrayCreate(1);
    for (size_t n = 0; n < 1; n++) {
        CPStringRef item = CPStringCreate(CPTEXT("123"));
        CPArrayAddItem(array1, item);
        CPRelease(item);
    }
    CPArrayRef array2 = CPArrayCreate(2);
    for (size_t n = 0; n < 2; n++) {
        CPStringRef item = CPStringCreate(CPTEXT("123"));
        CPArrayAddItem(array2, item);
        CPRelease(item);
    }
    CPArrayRef array3 = CPArrayCreate(3);
    for (size_t n = 0; n < 3; n++) {
        CPStringRef item = CPStringCreate(CPTEXT("123"));
        CPArrayAddItem(array3, item);
        CPRelease(item);
    }
    CPArrayRef arrayE = CPArrayCreate(3);
    for (size_t n = 0; n < 3; n++) {
        CPStringRef item = CPStringCreate(CPTEXT(""));
        CPArrayAddItem(arrayE, item);
        CPRelease(item);
    }

    string = CPStringCreateByCombiningStrings(array0, NULL);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT(""), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(array0, sep0);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT(""), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(array0, sep1);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT(""), 0));
    CPRelease(string);

    string = CPStringCreateByCombiningStrings(array1, NULL);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 3);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT("123"), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(array1, sep0);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 3);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT("123"), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(array1, sep1);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 3);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT("123"), 0));
    CPRelease(string);

    string = CPStringCreateByCombiningStrings(array2, NULL);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 2 * 3);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT("123123"), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(array2, sep0);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 2 * 3);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT("123123"), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(array2, sep1);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 2 * 3 + 1);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT("123,123"), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(array2, sep2);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 2 * 3 + 2);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT("123, 123"), 0));
    CPRelease(string);

    string = CPStringCreateByCombiningStrings(array3, NULL);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 3 * 3);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT("123123123"), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(array3, sep0);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 3 * 3);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT("123123123"), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(array3, sep1);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 3 * 3 + 2);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT("123,123,123"), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(array3, sep2);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 3 * 3 + 2 * 2);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT("123, 123, 123"), 0));
    CPRelease(string);

    string = CPStringCreateByCombiningStrings(arrayE, NULL);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT(""), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(arrayE, sep0);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT(""), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(arrayE, sep1);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT(""), 0));
    CPRelease(string);
    string = CPStringCreateByCombiningStrings(arrayE, sep2);
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringGetLength(string) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string, CPTEXT(""), 0));
    CPRelease(string);

    CPRelease(arrayE);
    CPRelease(array3);
    CPRelease(array2);
    CPRelease(array1);
    CPRelease(array0);
}

void test_string_gets()
{
    CPStringRef string;
    const CPChar* value;
    CPChar buffer1[1024];
    CPChar buffer2[1];
    CPChar buffer3[2];
    CPChar buffer4[17];

    string = NULL;
    CU_ASSERT_TRUE(CPStringIsNullOrEmpty(string));

    string = CPStringCreate(CPTEXT(""));
    CU_ASSERT(string != NULL);
    CU_ASSERT_TRUE(CPStringIsNullOrEmpty(string));
    CPRelease(string);

    string = CPStringCreate(CPTEXT("a"));
    CU_ASSERT(string != NULL);
    CU_ASSERT_FALSE(CPStringIsNullOrEmpty(string));
    CPRelease(string);

    string = CPStringCreate(CPTEXT(""));
    CU_ASSERT(string != NULL);
    CU_ASSERT_TRUE(CPStringIsNullOrEmpty(string));
    CU_ASSERT(CPStringGetLength(string) == 0);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 0);
    CU_ASSERT(CPStrCmp(value, CPTEXT("")) == 0);
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 0, CPCOUNT(buffer1)));
    CU_ASSERT(CPStrLen(buffer1) == 0);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("")) == 0);
    CPRelease(string);

    string = CPStringCreate(CPTEXT("a"));
    CU_ASSERT(string != NULL);
    CU_ASSERT_FALSE(CPStringIsNullOrEmpty(string));
    CU_ASSERT(CPStringGetLength(string) == 1);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 1);
    CU_ASSERT(CPStrCmp(value, CPTEXT("a")) == 0);
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 0, CPCOUNT(buffer1)));
    CU_ASSERT(CPStrLen(buffer1) == 1);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("a")) == 0);
    CPRelease(string);

    string = CPStringCreate(CPTEXT("12345678901234567890"));
    CU_ASSERT(string != NULL);
    CU_ASSERT_FALSE(CPStringIsNullOrEmpty(string));
    CU_ASSERT(CPStringGetLength(string) == 20);
    value = CPStringGet(string);
    CU_ASSERT(CPStrLen(value) == 20);
    CU_ASSERT(CPStrCmp(value, CPTEXT("12345678901234567890")) == 0);
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 0, CPCOUNT(buffer1)));
    CU_ASSERT(CPStrLen(buffer1) == 20);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("12345678901234567890")) == 0);
    CPRelease(string);

    // Substrings
    string = CPStringCreate(CPTEXT("12345678901234567890"));
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 0, CPCOUNT(buffer1)));
    CU_ASSERT(CPStrLen(buffer1) == 20);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("12345678901234567890")) == 0);
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 0, 10));
    CU_ASSERT(CPStrLen(buffer1) == 10);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("1234567890")) == 0);
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 0, 0));
    CU_ASSERT(CPStrLen(buffer1) == 0);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("")) == 0);
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 0, 1));
    CU_ASSERT(CPStrLen(buffer1) == 1);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("1")) == 0);
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 10, 10));
    CU_ASSERT(CPStrLen(buffer1) == 10);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("1234567890")) == 0);
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 10, 1));
    CU_ASSERT(CPStrLen(buffer1) == 1);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("1")) == 0);
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 9, 11));
    CU_ASSERT(CPStrLen(buffer1) == 11);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("01234567890")) == 0);
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 1, 1));
    CU_ASSERT(CPStrLen(buffer1) == 1);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("2")) == 0);
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 20, 0));
    CU_ASSERT(CPStrLen(buffer1) == 0);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("")) == 0);
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 19, 1));
    CU_ASSERT(CPStrLen(buffer1) == 1);
    CU_ASSERT(CPStrCmp(buffer1, CPTEXT("0")) == 0);
    CPRelease(string);

    // Try various ways of calling GetCharacters with weird buffers/args
    string = CPStringCreate(CPTEXT("12345678901234567890"));
    CPZeroMemory(buffer1, sizeof(buffer1), 0, sizeof(buffer1));
    CU_ASSERT_FALSE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 21, 0));
    CU_ASSERT_FALSE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 21, 1));
    CU_ASSERT_FALSE(CPStringGetCharacters(string, buffer1, sizeof(buffer1), 21, 20));
    CU_ASSERT_FALSE(CPStringGetCharacters(string, buffer2, 0, 0, 0));
    CU_ASSERT_FALSE(CPStringGetCharacters(string, buffer2, 0, 0, 1));
    CU_ASSERT_FALSE(CPStringGetCharacters(string, buffer2, 0, 1, 1));
    CPZeroMemory(buffer3, sizeof(buffer3), 0, sizeof(buffer3));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer3, sizeof(buffer3), 0, 1));
    CU_ASSERT(CPStrLen(buffer3) == 1);
    CU_ASSERT(CPStrCmp(buffer3, CPTEXT("1")) == 0);
    CPZeroMemory(buffer3, sizeof(buffer3), 0, sizeof(buffer3));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer3, sizeof(buffer3), 1, 1));
    CU_ASSERT(CPStrLen(buffer3) == 1);
    CU_ASSERT(CPStrCmp(buffer3, CPTEXT("2")) == 0);
    CU_ASSERT_FALSE(CPStringGetCharacters(string, buffer3, sizeof(buffer3), 0, 2));
    CU_ASSERT_FALSE(CPStringGetCharacters(string, buffer3, sizeof(buffer3), 1, 2));
    CPZeroMemory(buffer4, sizeof(buffer4), 0, sizeof(buffer4));
    CU_ASSERT_TRUE(CPStringGetCharacters(string, buffer4, sizeof(buffer4), 0, 16));
    CU_ASSERT(CPStrLen(buffer4) == 16);
    CU_ASSERT(CPStrCmp(buffer4, CPTEXT("1234567890123456")) == 0);
    CPZeroMemory(buffer4, sizeof(buffer4), 0, sizeof(buffer4));
    CU_ASSERT_FALSE(CPStringGetCharacters(string, buffer4, sizeof(buffer4), 0, 17));
    CPRelease(string);
}

void test_string_compares()
{
    CPStringRef string1;
    CPStringRef string2;
    const CPChar* string3;

    string1 = CPStringCreate(CPTEXT(""));
    string2 = CPStringCreate(CPTEXT(""));
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(string2 != NULL);
    CU_ASSERT(CPStringCompare(string1, string2, 0) == 0);
    CU_ASSERT(CPStringCompare(string1, string2, CPStringOptionsCaseInsensitive) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqual(string1, string2, 0));
    CU_ASSERT_TRUE(CPStringsAreEqual(string1, string2, CPStringOptionsCaseInsensitive));
    CPRelease(string2);
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT("a"));
    string2 = CPStringCreate(CPTEXT("a"));
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(string2 != NULL);
    CU_ASSERT(CPStringCompare(string1, string2, 0) == 0);
    CU_ASSERT(CPStringCompare(string1, string2, CPStringOptionsCaseInsensitive) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqual(string1, string2, 0));
    CU_ASSERT_TRUE(CPStringsAreEqual(string1, string2, CPStringOptionsCaseInsensitive));
    CPRelease(string2);
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT("a"));
    string2 = CPStringCreate(CPTEXT("A"));
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(string2 != NULL);
    CU_ASSERT(CPStringCompare(string1, string2, 0) != 0);
    CU_ASSERT(CPStringCompare(string1, string2, CPStringOptionsCaseInsensitive) == 0);
    CU_ASSERT_FALSE(CPStringsAreEqual(string1, string2, 0));
    CU_ASSERT_TRUE(CPStringsAreEqual(string1, string2, CPStringOptionsCaseInsensitive));
    CPRelease(string2);
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT("hello world!"));
    string2 = CPStringCreate(CPTEXT("hElLo WoRlD!"));
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(string2 != NULL);
    CU_ASSERT(CPStringCompare(string1, string2, 0) != 0);
    CU_ASSERT(CPStringCompare(string1, string2, CPStringOptionsCaseInsensitive) == 0);
    CU_ASSERT_FALSE(CPStringsAreEqual(string1, string2, 0));
    CU_ASSERT_TRUE(CPStringsAreEqual(string1, string2, CPStringOptionsCaseInsensitive));
    CPRelease(string2);
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT("hello world!"));
    string2 = CPStringCreate(CPTEXT("hello world!"));
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(string2 != NULL);
    CU_ASSERT(CPStringCompare(string1, string2, 0) == 0);
    CU_ASSERT(CPStringCompare(string1, string2, CPStringOptionsCaseInsensitive) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqual(string1, string2, 0));
    CU_ASSERT_TRUE(CPStringsAreEqual(string1, string2, CPStringOptionsCaseInsensitive));
    CPRelease(string2);
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT(""));
    string2 = CPStringCreate(CPTEXT("a"));
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(string2 != NULL);
    CU_ASSERT(CPStringCompare(string1, string2, 0) < 0);
    CU_ASSERT(CPStringCompare(string1, string2, CPStringOptionsCaseInsensitive) < 0);
    CU_ASSERT_FALSE(CPStringsAreEqual(string1, string2, 0));
    CU_ASSERT_FALSE(CPStringsAreEqual(string1, string2, CPStringOptionsCaseInsensitive));
    CPRelease(string2);
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT("a"));
    string2 = CPStringCreate(CPTEXT(""));
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(string2 != NULL);
    CU_ASSERT(CPStringCompare(string1, string2, 0) > 0);
    CU_ASSERT(CPStringCompare(string1, string2, CPStringOptionsCaseInsensitive) > 0);
    CU_ASSERT_FALSE(CPStringsAreEqual(string1, string2, 0));
    CU_ASSERT_FALSE(CPStringsAreEqual(string1, string2, CPStringOptionsCaseInsensitive));
    CPRelease(string2);
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT("a"));
    string2 = CPStringCreate(CPTEXT("A"));
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(string2 != NULL);
    CU_ASSERT(CPStringCompare(string1, string2, 0) > 0);
    CU_ASSERT(CPStringCompare(string1, string2, CPStringOptionsCaseInsensitive) == 0);
    CU_ASSERT_FALSE(CPStringsAreEqual(string1, string2, 0));
    CU_ASSERT_TRUE(CPStringsAreEqual(string1, string2, CPStringOptionsCaseInsensitive));
    CPRelease(string2);
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT("A"));
    string2 = CPStringCreate(CPTEXT("a"));
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(string2 != NULL);
    CU_ASSERT(CPStringCompare(string1, string2, 0) < 0);
    CU_ASSERT(CPStringCompare(string1, string2, CPStringOptionsCaseInsensitive) == 0);
    CU_ASSERT_FALSE(CPStringsAreEqual(string1, string2, 0));
    CU_ASSERT_TRUE(CPStringsAreEqual(string1, string2, CPStringOptionsCaseInsensitive));
    CPRelease(string2);
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT(""));
    string3 = CPTEXT("");
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(CPStringCompareChar(string1, string3, 0) == 0);
    CU_ASSERT(CPStringCompareChar(string1, string3, CPStringOptionsCaseInsensitive) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string1, string3, 0));
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string1, string3, CPStringOptionsCaseInsensitive));
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT("a"));
    string3 = CPTEXT("a");
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(CPStringCompareChar(string1, string3, 0) == 0);
    CU_ASSERT(CPStringCompareChar(string1, string3, CPStringOptionsCaseInsensitive) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string1, string3, 0));
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string1, string3, CPStringOptionsCaseInsensitive));
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT("a"));
    string3 = CPTEXT("A");
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(CPStringCompareChar(string1, string3, 0) != 0);
    CU_ASSERT(CPStringCompareChar(string1, string3, CPStringOptionsCaseInsensitive) == 0);
    CU_ASSERT_FALSE(CPStringsAreEqualChar(string1, string3, 0));
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string1, string3, CPStringOptionsCaseInsensitive));
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT("hello world!"));
    string3 = CPTEXT("hello world!");
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(CPStringCompareChar(string1, string3, 0) == 0);
    CU_ASSERT(CPStringCompareChar(string1, string3, CPStringOptionsCaseInsensitive) == 0);
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string1, string3, 0));
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string1, string3, CPStringOptionsCaseInsensitive));
    CPRelease(string1);

    string1 = CPStringCreate(CPTEXT("hello world!"));
    string3 = CPTEXT("hElLo WoRlD!");
    CU_ASSERT(string1 != NULL);
    CU_ASSERT(CPStringCompareChar(string1, string3, 0) != 0);
    CU_ASSERT(CPStringCompareChar(string1, string3, CPStringOptionsCaseInsensitive) == 0);
    CU_ASSERT_FALSE(CPStringsAreEqualChar(string1, string3, 0));
    CU_ASSERT_TRUE(CPStringsAreEqualChar(string1, string3, CPStringOptionsCaseInsensitive));
    CPRelease(string1);
}

void test_string_search()
{
    CPStringRef string;

    string = CPStringCreate(CPTEXT(""));
    CU_ASSERT(string != NULL);
    CU_ASSERT_FALSE(CPStringHasPrefix(string, CPTEXT("")));
    CU_ASSERT_FALSE(CPStringHasSuffix(string, CPTEXT("")));
    CU_ASSERT_FALSE(CPStringHasPrefix(string, CPTEXT("a")));
    CU_ASSERT_FALSE(CPStringHasSuffix(string, CPTEXT("a")));
    CPRelease(string);

    string = CPStringCreate(CPTEXT("hello world!"));
    CU_ASSERT(string != NULL);
    CU_ASSERT_TRUE(CPStringHasPrefix(string, CPTEXT("h")));
    CU_ASSERT_TRUE(CPStringHasPrefix(string, CPTEXT("hello")));
    CU_ASSERT_FALSE(CPStringHasPrefix(string, CPTEXT("")));
    CU_ASSERT_FALSE(CPStringHasPrefix(string, CPTEXT("hElLo")));
    CU_ASSERT_FALSE(CPStringHasPrefix(string, CPTEXT("helloX")));
    CU_ASSERT_FALSE(CPStringHasPrefix(string, CPTEXT("ello")));
    CU_ASSERT_TRUE(CPStringHasSuffix(string, CPTEXT("!")));
    CU_ASSERT_TRUE(CPStringHasSuffix(string, CPTEXT("world!")));
    CU_ASSERT_FALSE(CPStringHasSuffix(string, CPTEXT("")));
    CU_ASSERT_FALSE(CPStringHasSuffix(string, CPTEXT("wOrLd!")));
    CU_ASSERT_FALSE(CPStringHasSuffix(string, CPTEXT("Xworld!")));
    CU_ASSERT_FALSE(CPStringHasSuffix(string, CPTEXT("world")));
    CPRelease(string);

    string = CPStringCreate(CPTEXT(""));
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringFind(string, CPTEXT("")) == CPStringInvalidIndex);
    CU_ASSERT(CPStringFind(string, CPTEXT("hello")) == CPStringInvalidIndex);
    CPRelease(string);

    string = CPStringCreate(CPTEXT("hello world!"));
    CU_ASSERT(string != NULL);
    CU_ASSERT(CPStringFind(string, CPTEXT("hello")) == 0);
    CU_ASSERT(CPStringFind(string, CPTEXT("h")) == 0);
    CU_ASSERT(CPStringFind(string, CPTEXT(" ")) == 5);
    CU_ASSERT(CPStringFind(string, CPTEXT("!")) == 11);
    CU_ASSERT(CPStringFind(string, CPTEXT("hello world!")) == 0);
    CU_ASSERT(CPStringFind(string, CPTEXT("")) == CPStringInvalidIndex);
    CU_ASSERT(CPStringFind(string, CPTEXT("hElLo")) == CPStringInvalidIndex);
    CU_ASSERT(CPStringFind(string, CPTEXT("hello world!X")) == CPStringInvalidIndex);
    CPRelease(string);
}

static CU_TestInfo test_string_infos[] = {
    { "test_string_create",             test_string_create              },
    { "test_string_createsubstring",    test_string_createsubstring     },
    { "test_string_createformat",       test_string_createformat        },
    { "test_string_createcopy",         test_string_createcopy          },
    { "test_string_createcombining",    test_string_createcombining     },
    { "test_string_gets",               test_string_gets                },
    { "test_string_compares",           test_string_compares            },
    { "test_string_search",             test_string_search              },
};

CU_REGISTER_SUITE(test_string);
