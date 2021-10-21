#include "chitajson.h"
#include <assert.h>     /* assert() */
#include <errno.h>      /* errno, ERANGE */
#include <math.h>       /* HUGE_VAL */
#include <stdlib.h>     /* NULL, strtod() */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')

typedef struct {    
    const char* json;
}chita_context;

/* ws = *(%x20 / %x09 / %x0A / %x0D) */
static void chita_parse_whitespace(chita_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int chita_parse_literal(chita_context* c, chita_value* v, const char* literal,chita_type type){
    size_t i;
    EXPECT(c,literal[0]);
    for(i = 0;literal[i+1];i++){
        if(c->json[i]!=literal[i+1]){
            return CHITA_PARSE_INVALID_VALUE;
        }
    }
    c->json += i;
    v->type = type;
    return CHITA_PARSE_OK;
}

static int chita_parse_number(chita_context* c,chita_value* v){
    /*  整数  ... */
    const char* p = c->json;
    /*  负号  ... */
    if(*p == '-') p++;
    if(*p == '0'){p++;}
    else{
        if(!ISDIGIT1TO9(*p)) return CHITA_PARSE_INVALID_VALUE;
        for(p++;ISDIGIT(*p);p++);
    }
    /*  小数  ... */
    if(*p == '.'){
        p++;
        if(!ISDIGIT(*p)) return CHITA_PARSE_INVALID_VALUE;
        for(p++;ISDIGIT(*p);p++);
    }
    /*  指数  ... */
    if(*p == 'e'|| *p == 'E'){
        p++;
        if(*p == '+' || *p == '-') p++;
        if(!ISDIGIT(*p)) return CHITA_PARSE_INVALID_VALUE;
        for(p++;ISDIGIT(*p);p++);
    }
    errno = 0;
    v->n = strtod(c->json,NULL);
    if(errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL)){
        return CHITA_PARSE_NUMBER_TOO_BIG;
    }
    v->type = CHITA_NUMBER;
    c->json = p;
    return CHITA_PARSE_OK;
}

static int chita_parse_value(chita_context* c, chita_value* v) {
    switch (*c->json) {
        case 't':  return chita_parse_literal(c, v,"true",CHITA_TRUE);
        case 'f':  return chita_parse_literal(c, v,"false", CHITA_FALSE);
        case 'n':  return chita_parse_literal(c, v,"null",CHITA_NULL);
        case '\0': return CHITA_PARSE_EXPECT_VALUE;
        default:   return chita_parse_number(c,v);
    }
}


int chita_parse(chita_value* v, const char* json) {
    chita_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = CHITA_NULL;
    chita_parse_whitespace(&c);
    if((ret = chita_parse_value(&c,v))==CHITA_PARSE_OK){
        chita_parse_whitespace(&c);
        if(*c.json != '\0'){
            v->type = CHITA_NULL;
            ret = CHITA_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

chita_type chita_get_type(const chita_value* v) {
    assert(v != NULL);
    return v->type;
}

double chita_get_number(const chita_value* v){
    assert(v!=NULL&&v->type == CHITA_NUMBER);
    return v->n;
}


