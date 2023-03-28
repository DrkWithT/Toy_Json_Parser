/**
 * @file myjson.c
 * @author Derek Tan (DrkWithT at GitHub)
 * @brief Test driver code for my JSON parser.
 * @date 2023-03-25
 */

// #include "json_thing.h" // uncomment after parser is done
#include "json_parser.h"

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

    // // show token data:
    // for (size_t i = 0; i < tokens->capacity; i++)
    //     Print_Token(i, TokenVec_At(tokens, i));

    // test Parser!
    DataType target_type = UNSUPPORTED;
    char *old_buf_ref = Lexer_CleanUp(lexer_ref);
    Parser *parser_ref = Parser_Create(old_buf_ref, tokens);

    puts("Testing parser...");
    JsonThing *json_ds = Parser_Start_Parse(parser_ref); 

    printf("parser exit code (should be 0): %i\n", Parser_Get_ErrCode(parser_ref));
    Parser_Reset(parser_ref);

    puts("Testing object property:");
    if (json_ds != NULL)
    {
        if (strcmp(argv[1], "tests/test1.json") == 0)
        {
            Object *obj = (Object*)json_ds->root->data.chunk;
            printf("root obj: %p\n", obj);

            Array *clubs = (Array*)Property_AsChunk(Object_GetItem(obj, "clubs"), &target_type);
            printf("clubs: %p, type: %i\n", clubs, target_type);

            const ArrayItem *item1 = Array_Get(clubs, 0);
            puts("fetch &clubs[0]");

            printf("json_ds[\"clubs\"][0] = \"%s\"\n", item1->data.str);
        }
        else if (strcmp(argv[1], "tests/test2.json") == 0)
        {
            Array *arr = (Array*)json_ds->root->data.chunk;
            printf("json_ds[0] = %i\n", Array_Get(arr, 0)->data.i);
        }
    }

    // free all dynamic memory:
    free(old_buf_ref);
    old_buf_ref = NULL;
    puts("Freed src.");

    TokenVec_Destroy(tokens);
    free(tokens);
    tokens = NULL;
    puts("Freed tokens.");

    free(lexer_ref);
    lexer_ref = NULL;
    puts("Freed Lexer.");

    free(parser_ref);
    parser_ref = NULL;
    puts("Freed Parser.");

    JsonThing_Destroy(json_ds);
    free(json_ds);
    json_ds = NULL;
    puts("Freed json thing.");

    return 0;
}