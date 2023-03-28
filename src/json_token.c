/**
 * @file json_token.c
 * @author Derek Tan (DrkWithT at GitHub)
 * @brief Implements Token and TokenVec.
 * @date 2023-03-26
 */

#include "json_token.h"

/// Token:

Token *Token_Create(TokenType _type, size_t _begin, size_t _span)
{
    Token *result = malloc(sizeof(Token));

    if (!result)
        return result;
    
    result->type = _type;
    result->begin = _begin;
    result->span = _span;

    return result;
}

char *Token_ToTxt(const Token *self, const char *src)
{
    size_t buf_pos = 0;
    size_t src_pos = self->begin;
    size_t real_sz = self->span + 1; // include space for null terminator

    char *txt = malloc(sizeof(char) * real_sz);

    if (!txt)
        return txt;

    for (size_t count = 0; count < real_sz - 1; count++)
    {
        txt[buf_pos] = src[src_pos];
        buf_pos++;
        src_pos++;
    }

    txt[buf_pos] = '\0';

    return txt;
}

/// TokenVec:

TokenVec *TokenVec_Create(size_t _capacity)
{
    TokenVec *result = malloc(sizeof(TokenVec));

    if (!result)
        return result;
    
    result->capacity = _capacity;
    result->count = 0;
    result->data = malloc(sizeof(Token*) * _capacity);

    if (!result->data)
    {
        result->capacity = 0;
    }
    else
    {
        for (size_t i = 0; i < result->capacity; i++)
            result->data[i] = NULL; // initialize empty token slots
    }

    return result;
}

void TokenVec_Destroy(TokenVec *self)
{
    if (!self->data)
        return;
    
    // free all referenced tokens
    for (size_t i = 0; i < self->capacity; i++)
    {
        if (!self->data[i])
            continue;
        
        free(self->data[i]);
    }
    
    // free emptied token buffer
    free(self->data);
    self->data = NULL;
    self->capacity = 0;
}

void TokenVec_Grow(TokenVec *self)
{
    if (!self->data)
        return;

    size_t new_capacity = self->capacity << 1;
    size_t old_end_pos = self->capacity;

    Token **temp = realloc(self->data, (sizeof(Token*) * new_capacity));

    if (!temp)
        return;

    self->data = temp;
    self->capacity = new_capacity;
    // slot usage count is unchanged!

    // initialize new empty slots after the older ones...
    for (size_t i = old_end_pos; i < new_capacity; i++)
        self->data[i] = NULL;
}

void TokenVec_Set(TokenVec *self, size_t idx, Token *item)
{
    // only set an empty slot and update length if ok
    if (!self->data[idx])
    {
        self->data[idx] = item;
        self->count++;
    }

    // also reallocate this vector when old slots are full
    if (self->count == self->capacity)
        TokenVec_Grow(self);
}

Token *TokenVec_At(TokenVec *self, size_t idx)
{
    return self->data[idx];
}
