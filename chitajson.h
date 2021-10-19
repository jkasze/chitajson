#ifndef CHITAJSON_H__
#define CHITAJSON_H__

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
    chita_type type;
} chita_value;

enum {
    CHITA_PARSE_OK = 0,             /* JSON正常返回            */
    CHITA_PARSE_EXPECT_VALUE,       /* JSON只含有空白           */
    CHITA_PARSE_INVALID_VALUE,      /* JSON在空白之后还有其他字符 */
    CHITA_PARSE_ROOT_NOT_SINGULAR   /* 非三种字面值             */
};
int chita_parse(chita_value* v, const char* json);

chita_type chita_get_type(const chita_value* v);

#endif /* CHITAJSON_H__ */
