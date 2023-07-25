/**
 * @file myjson.c
 * @author (DrkWithT at GitHub)
 * @brief Test driver code for my JSON parser.
 * @date 2023-03-25
 */

#include "json_parser.h"

#define TEST_COUNT 3

static const char TEST_FILES[TEST_COUNT][17] = {
    "tests/test1.json",
    "tests/test2.json",
    "tests/test3.json"
};

void Do_Test1(const JsonThing *json_ds)
{
    DataType type = UNSUPPORTED;

    Object *obj = (Object*)json_ds->root->data.chunk;
    Array *clubs = (Array*)Property_AsChunk(Object_GetItem(obj, "clubs"), &type);
    const ArrayItem *item1 = Array_Get(clubs, 0);

    printf("type %i: json_ds[\"clubs\"][0] = \"%s\"\n", type, item1->data.str);
}

void Do_Test2(const JsonThing *json_ds)
{
    Array *arr = (Array*)json_ds->root->data.chunk;

    printf("json_ds[0] = %i\n", Array_Get(arr, 0)->data.i);
}

void Do_Test3(const JsonThing *json_ds)
{
    Array *arr = (Array*)json_ds->root->data.chunk;
    Object *obj1 = (Object*)Array_Get(arr, 1)->data.chunk;
    const Property *coord_x = (Property*)Object_GetItem(obj1, "x");

    printf("json_ds[1][\"x\"] = %i\n", coord_x->data.i);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: ./myjson <test number>\n");
        return 1;
    }

    int test_index = atoi(argv[1]) - 1;

    if (test_index < 0 || test_index >= TEST_COUNT)
    {
        printf("Invalid test number.\n");
        return 1;
    }

    Lexer *lexer_ref = Lexer_Create(TEST_FILES[test_index]);

    if (!lexer_ref)
    {
        printf("Failed to create Lexer!\n");
        return 1;
    }

    TokenVec *tokens = Lexer_Lex_All(lexer_ref);
    char *old_buf_ref = Lexer_CleanUp(lexer_ref);

    Parser *parser_ref = Parser_Create(old_buf_ref, tokens);

    JsonThing *json_result = Parser_Start_Parse(parser_ref); 

    printf("parser exit code (should be 0): %i\n", Parser_Get_ErrCode(parser_ref));
    Parser_Reset(parser_ref);

    puts("Testing object property:");
    if (json_result != NULL)
    {
        switch (test_index)
        {
        case 0:
            Do_Test1(json_result);
            break;
        case 1:
            Do_Test2(json_result);
            break;
        case 2:
            Do_Test3(json_result);
            break;
        default:
            break;
        }
    }

    if (old_buf_ref != NULL)
    {
        free(old_buf_ref);
        old_buf_ref = NULL;
    }

    if (tokens != NULL)
    {
        TokenVec_Destroy(tokens);
        free(tokens);
        tokens = NULL;
    }

    if (lexer_ref != NULL)
    {
        free(lexer_ref);
        lexer_ref = NULL;
    }

    if (parser_ref != NULL)
    {
        free(parser_ref);
        parser_ref = NULL;
    }

    if (json_result != NULL)
    {
        JsonThing_Destroy(json_result);
        free(json_result);
        json_result = NULL;
    }

    puts("Cleanup OK");

    return 0;
}