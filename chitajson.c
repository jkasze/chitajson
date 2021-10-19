#include "chitajson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}chita_context;

static void chita_parse_whitespace(chita_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int chita_parse_null(chita_context* c, chita_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return CHITA_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = CHITA_NULL;
    return CHITA_PARSE_OK;
}

static int chita_parse_value(chita_context* c, chita_value* v) {
    switch (*c->json) {
        case 'n':  return chita_parse_null(c, v);
        case '\0': return CHITA_PARSE_EXPECT_VALUE;
        default:   return CHITA_PARSE_INVALID_VALUE;
    }
}

int chita_parse(chita_value* v, const char* json) {
    chita_context c;
    assert(v != NULL);
    c.json = json;
    v->type = CHITA_NULL;
    chita_parse_whitespace(&c);
    return chita_parse_value(&c, v);
}

chita_type chita_get_type(const chita_value* v) {
    assert(v != NULL);
    return v->type;
}
