/**
 * @file myjson.c
 * @author Derek Tan (DrkWithT at GitHub)
 * @brief Test driver code for my JSON parser.
 * @date 2023-03-25
 */

// #include "json_thing.h" // uncomment after parser is done
#include "json_lex.h"
#include <stdio.h>

// Token typenames:
static const char TOKEN_TYPENAMES[13][8] = {
    "WTSPACE",
    "L_BRACK",
    "R_BRACK",
    "L_CURLY",
    "R_CURLY",
    "STRBODY",
    "COLON_P",
    "INT_LTL",
    "FLT_LTL",
    "NLL_LTL",
    "COMMA_P",
    "EOF_TOK",
    "UNKNOWN"
};

/// Debug Helpers

void Print_Token(size_t t_num, const Token *t)
{
    if (t != NULL)
        printf("Token %zu: type=%s, begin=%zu, span=%zu\n", t_num, TOKEN_TYPENAMES[t->type], t->begin, t->span);
    else
        printf("NULL\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: ./myjson <file name>\n");
        return 1;
    }

    // try to create lexer successfully:
    Lexer *lexer_ref = Lexer_Create(argv[1]);

    if (!lexer_ref)
    {
        printf("Failed to create Lexer!\n");
        return 1;
    }

    // test Lexer!
    TokenVec *tokens = Lexer_Lex_All(lexer_ref);

    size_t slot_count = tokens->capacity;

    // show token data:
    for (size_t i = 0; i < slot_count; i++)
        Print_Token(i, TokenVec_At(tokens, i));

    // free all dynamic memory:
    TokenVec_Destroy(tokens);
    free(tokens);
    tokens = NULL;

    char *old_buf_ref = Lexer_CleanUp(lexer_ref);
    free(old_buf_ref);
    old_buf_ref = NULL;

    free(lexer_ref);
    lexer_ref = NULL;

    return 0;
}