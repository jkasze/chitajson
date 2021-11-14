#ifndef CHITAJSON_H__
#define CHITAJSON_H__

#include <stddef.h> /* size_t */

typedef enum { 
    CHITA_NULL, 
    CHITA_FALSE, 
    CHITA_TRUE, 
    CHITA_NUMBER, 
    CHITA_STRING, 
    CHITA_ARRAY, 
    CHITA_OBJECT 
} chita_type;

typedef struct {
    union{
        struct{ char* s;size_t len;}s;  /* string */
        double n;                       /* number */
    }u;
    chita_type type;
} chita_value;


enum {
    CHITA_PARSE_OK = 0,             /* JSON正常返回            */
    CHITA_PARSE_EXPECT_VALUE,       /* JSON只含有空白           */
    CHITA_PARSE_INVALID_VALUE,      /* JSON在空白之后还有其他字符 */
    CHITA_PARSE_ROOT_NOT_SINGULAR,  /* 非三种字面值             */
    CHITA_PARSE_NUMBER_TOO_BIG,      /* */
    CHITA_PARSE_MISS_QUOTATION_MARK,
    CHITA_PARSE_INVALID_STRING_ESCAPE,
    CHITA_PARSE_INVALID_STRING_CHAR
};

#define chita_init(v) do {(v)->type = CHITA_NULL; } while(0)

int chita_parse(chita_value* v, const char* json);

void chita_free(chita_value* v);

chita_type chita_get_type(const chita_value* v);

#define chita_set_null(v) chita_free(v)

/*  boolean */
int chita_get_boolean(const chita_value* v);
void chita_set_boolean(chita_value* v, int b);

/*  number  */
double chita_get_number(const chita_value* v);
void chita_set_number(chita_value* v,double n);

/*  string  */
const char* chita_get_string(const chita_value* v);
size_t chita_get_string_length(const chita_value* v);
void chita_set_string(chita_value* v, const char* s,size_t len);

#endif /* CHITAJSON_H__ */
