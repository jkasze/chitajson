#include "chitajson.h"
#include <assert.h>     /* assert() */
#include <errno.h>      /* errno, ERANGE */
#include <math.h>       /* HUGE_VAL */
#include <stdlib.h>     /* NULL, strtod() */
#include <string.h>     /* memcpy() */

#ifndef CHITA_PARSE_STACK_INIT_SIZE
#define CHITA_PARSE_STACK_INIT_SIZE 256
#endif

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')
#define PUTC(c,ch) do{*(char*)chita_context_push(c,sizeof(char)) = (ch);} while(0) 



typedef struct {    
    const char* json;
    char* stack;
    size_t size,top;
}chita_context;

static void* chita_context_push(chita_context* c, size_t size){
    void* ret;
    assert(size > 0);
    if(c->top + size >= c-> size){
        if(c->size == 0){
            c->size = CHITA_PARSE_STACK_INIT_SIZE;
        }
        while(c->top + size >= c->size){
            c->size += c->size >> 1; /* c->size * 1.5 */

        }
        c->stack = (char*)realloc(c->stack, c->size);
    }
    ret = c->stack + c->top;
    c->top += size;
    return ret;
}

static void* chita_context_pop(chita_context* c, size_t size){
    assert(c->top >= size);
    return c->stack + (c->top -= size);
}

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
    v->u.n = strtod(c->json,NULL);
    if(errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL)){
        return CHITA_PARSE_NUMBER_TOO_BIG;
    }
    v->type = CHITA_NUMBER;
    c->json = p;
    return CHITA_PARSE_OK;
}

static int chita_parse_string(chita_context* c, chita_value* v){
    size_t head = c->top,len;
    const char* p;
    EXPECT(c,'\"');
    p = c->json;
    for(;;){
        char ch = *p++;
        switch(ch){
            case '\\':
                switch(*p++){
                    case '\"': PUTC(c,'\"');break;
                    case '\\': PUTC(c,'\\');break;
                    case '/':  PUTC(c,'/' );break;
                    case 'b':  PUTC(c,'\b');break;
                    case 'f':  PUTC(c,'\f');break;
                    case 'n':  PUTC(c,'\n');break;
                    case 'r':  PUTC(c,'\t');break;
                    case 't':  PUTC(c,'\t');break;
                    default:
                        c->top = head;
                        return CHITA_PARSE_INVALID_STRING_ESCAPE;
                }
            case '\"':
                len = c->top - head;
                chita_set_string(v,(const char*)chita_context_pop(c,len),len);
                c->json = p;
                return CHITA_PARSE_OK;
            case '\0':
                c->top = head;
                return CHITA_PARSE_MISS_QUOTATION_MARK;
            default:
                if ((unsigned char)ch < 0x20) { 
                    c->top = head;
                    return CHITA_PARSE_INVALID_STRING_CHAR;
                }
                PUTC(c, ch);
        }
    }
}

static int chita_parse_value(chita_context* c, chita_value* v) {
    switch (*c->json) {
        case 't':  return chita_parse_literal(c, v,"true",CHITA_TRUE);
        case 'f':  return chita_parse_literal(c, v,"false", CHITA_FALSE);
        case 'n':  return chita_parse_literal(c, v,"null",CHITA_NULL);
        case '\0': return CHITA_PARSE_EXPECT_VALUE;
        case '"':  return chita_parse_string(c,v);
        default:   return chita_parse_number(c,v);
    }
}


int chita_parse(chita_value* v, const char* json) {
    chita_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    c.stack = NULL;         /* <- */
    c.size = c.top = 0;     /* <- */
    chita_init(v);
    chita_parse_whitespace(&c);
    if((ret = chita_parse_value(&c,v))==CHITA_PARSE_OK){
        chita_parse_whitespace(&c);
        if(*c.json != '\0'){
            v->type = CHITA_NULL;
            ret = CHITA_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    assert(c.top == 0);     /* <- */
    free(c.stack);          /* <- */
    return ret;
}

void chita_free(chita_value* v){
    assert(v != NULL);
    if(v->type == CHITA_STRING){
        free(v->u.s.s);
    }
    v->type = CHITA_NULL;
}


chita_type chita_get_type(const chita_value* v) {
    assert(v != NULL);
    return v->type;
}
 
int chita_get_boolean(const chita_value* v){
    assert(v!=NULL && (v->type == CHITA_TRUE || v->type ==CHITA_FALSE));
    return v->type == CHITA_TRUE;
}

void chita_set_boolean(chita_value* v,int b){
    chita_free(v);
    v->type = b ? CHITA_TRUE : CHITA_FALSE;
}

double chita_get_number(const chita_value* v){
    assert(v!=NULL&&v->type == CHITA_NUMBER);
    return v->u.n;
}

void chita_set_number(chita_value* v, double n){
    chita_free(v);
    v->u.n = n;
    v->type = CHITA_NUMBER;
}

const char* chita_get_string(const chita_value* v){
    assert(v!=NULL && v->type ==CHITA_STRING);
    return v->u.s.s;
}

size_t chita_get_string_length(const chita_value* v){
    assert(v != NULL && v->type ==CHITA_STRING);
    return v->u.s.len;
}
void chita_set_string(chita_value* v,const char* s, size_t len){
    assert(v != NULL && (s!=NULL||len==0));
    chita_free(v);
    v->u.s.s = (char*)malloc(len+1);
    memcpy(v->u.s.s,s,len);
    v->u.s.s[len] = '\0';
    v->u.s.len = len;
    v->type = CHITA_STRING;
}