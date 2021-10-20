#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chitajson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")

#define TEST_ERROR(error,json)\
    do{\
        chita_value v;\
        v.type = CHITA_FALSE;\
        EXPECT_EQ_INT(error,chita_parse(&v,json));\
        EXPECT_EQ_INT(CHITA_NULL,chita_get_type(&v));\
    } while(0)

static void test_parse_null() {
    chita_value v;
    v.type = CHITA_FALSE;
    EXPECT_EQ_INT(CHITA_PARSE_OK, chita_parse(&v, "null"));
    EXPECT_EQ_INT(CHITA_NULL, chita_get_type(&v));
}



static void test_parse_expect_value() {
    chita_value v;

    v.type = CHITA_FALSE;
    EXPECT_EQ_INT(CHITA_PARSE_EXPECT_VALUE, chita_parse(&v, ""));
    EXPECT_EQ_INT(CHITA_NULL, chita_get_type(&v));

    v.type = CHITA_FALSE;
    EXPECT_EQ_INT(CHITA_PARSE_EXPECT_VALUE, chita_parse(&v, " "));
    EXPECT_EQ_INT(CHITA_NULL, chita_get_type(&v));
}

static void test_parse_invalid_value() {
    chita_value v;
    v.type = CHITA_FALSE;
    EXPECT_EQ_INT(CHITA_PARSE_INVALID_VALUE, chita_parse(&v, "nul"));
    EXPECT_EQ_INT(CHITA_NULL, chita_get_type(&v));

    v.type = CHITA_FALSE;
    EXPECT_EQ_INT(CHITA_PARSE_INVALID_VALUE, chita_parse(&v, "?"));
    EXPECT_EQ_INT(CHITA_NULL, chita_get_type(&v));
}

static void test_parse_root_not_singular() {
    chita_value v;
    v.type = CHITA_FALSE;
    EXPECT_EQ_INT(CHITA_PARSE_ROOT_NOT_SINGULAR, chita_parse(&v, "null x"));
    EXPECT_EQ_INT(CHITA_NULL, chita_get_type(&v));
}

static void test_parse_true(){
    chita_value v;
    v.type = CHITA_FALSE;
    EXPECT_EQ_INT(CHITA_PARSE_OK,chita_parse(&v,"true"));
    EXPECT_EQ_INT(CHITA_TRUE,chita_get_type(&v));
}
static void test_parse_false(){
    chita_value v;
    v.type = CHITA_TRUE;
    EXPECT_EQ_INT(CHITA_PARSE_OK,chita_parse(&v,"false"));
    EXPECT_EQ_INT(CHITA_FALSE,chita_get_type(&v));
}
static void test_parse() {
    test_parse_null();
    test_parse_true();    
    test_parse_false();   
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
}

int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
