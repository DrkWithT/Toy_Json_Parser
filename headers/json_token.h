#ifndef JSON_TOKEN_H
#define JSON_TOKEN_H

#include <stdlib.h>
#include <stdio.h>

/// Enums:

typedef enum token_type {
    LBRACKET,
    RBRACKET,
    LCURLY,
    RCURLY,
    STRBODY,
    COLON,
    INT_LTRL,     // text of [0-9]+ pattern
    FLT_LTRL,     // text of [0-9]*.[0-9]+ pattern
    NULL_LTRL,    // null keyword
    COMMA,
    FILE_END,     // eof token
    UNKNOWN       // unsupported thing
} TokenType;

/// Token & TokenVec:

typedef struct json_token
{
    TokenType type;
    size_t begin;
    size_t span;
} Token;

Token *Token_Create(TokenType _type, size_t _begin, size_t _span); // tokens should be externally freed...
char *Token_ToTxt(const Token *self, const char *src);

typedef struct token_vec
{
    Token **data;
    size_t count;
    size_t capacity;
} TokenVec;

TokenVec *TokenVec_Create(size_t _capacity);
void TokenVec_Destroy(TokenVec *self);
void TokenVec_Grow(TokenVec *self);
void TokenVec_Set(TokenVec *self, size_t idx, Token *item);
Token *TokenVec_At(TokenVec *self, size_t idx);

#endif
