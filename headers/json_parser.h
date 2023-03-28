#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "json_thing.h"
#include "json_lex.h"

/// Enums:

typedef enum json_parser_error {
    NO_ERR,
    EMPTY_TOKENS_ERR,
    UNEXPECTED_TOKEN_ERR, // token is incorrectly placed or typed
    UNKNOWN_TOKEN_ERR,    // token has invalid content
    UNBALANCED_NEST       // tokens have unbalanced sequence of [], {}
} ParserErr;

/// Recursive Parser:

typedef struct json_parser
{
    /* State & Data */

    ParserErr err_code;
    char *srcbuf_ref;      // references json text
    TokenVec *tokvec_ref;  // references json tokens
    size_t tokvec_idx;
    size_t tokvec_end;

    /* Parsing Temps */

    Property *temp_root; // parent prop. of entire JSON!
} Parser;

Parser *Parser_Create(char *src, TokenVec *tokens);
void Parser_Reset(Parser *self);
int Parser_IsReady(const Parser *self);
int Parser_AtEnd(const Parser *self);
Token *Parser_Peek_Token(Parser *self, size_t post_offset);

/**
 * @brief Parses a non-property primitive value. This covers standalone primitives or array items.
 * 
 * @param self
 * @param prim_type
 * @param relation
 * @note See JsonProx in json_types.h to know the role of param relation.
 * @return void*
 */
void *Parser_Parse_Prim(Parser *self, char *optional_name, DataType prim_type, JsonProx relation);

void *Parser_Parse_Arr(Parser *self);
void *Parser_Parse_Obj(Parser *self);
int Parser_Get_ErrCode(const Parser *self);
JsonThing *Parser_Start_Parse(Parser *self);

#endif