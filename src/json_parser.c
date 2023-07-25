/**
 * @file json_parser.c
 * @author (DrkWithT at GitHub)
 * @brief Implements the recursive Parser.
 * @date 2023-03-27
 */

#include "json_parser.h"

Parser *Parser_Create(char *src, TokenVec *tokens)
{
    Parser *result = malloc(sizeof(Parser));

    if (!result || !tokens)
        return result;
    
    result->srcbuf_ref = src;
    result->tokvec_ref = tokens;
    result->tokvec_idx = 0;
    result->tokvec_end = result->tokvec_ref->count; // remember to stop at a NULL terminator token!

    result->temp_root = NULL; // set this when parsing outermost JSON layer: primitive, array, or object!

    return result;
}

void Parser_Reset(Parser *self)
{
    // NOTE: unbind reference pointers, but make sure to get them before calling this function.
    self->err_code = NO_ERR;
    self->srcbuf_ref = NULL;
    self->tokvec_ref = NULL;
    self->temp_root = NULL;
    self->tokvec_idx = 0;
    self->tokvec_end = 0;
}

int Parser_IsReady(const Parser *self)
{
    // NOTE: if we have text, token slices, and some tokens, parsing can go!
    return self->srcbuf_ref != NULL && self->tokvec_ref != NULL && !Parser_AtEnd(self);
}

int Parser_AtEnd(const Parser *self)
{
    return self->tokvec_idx >= self->tokvec_end;
}

void *Parser_Parse_Prim(Parser *self, char *optional_name, DataType prim_type, JsonProx relation)
{
    void *temp = NULL;
    Token *curr_token_ref = NULL;
    char *curr_token_txt = NULL;
    curr_token_ref = TokenVec_At(self->tokvec_ref, self->tokvec_idx);
    
    if (!curr_token_ref)
        return temp;
    
    curr_token_txt = Token_ToTxt(curr_token_ref, self->srcbuf_ref);

    if (!curr_token_txt)
        return temp;
    
    TokenType tok_type = curr_token_ref->type;

    if (relation == TO_NONE)  // handle root constants
    {
        switch (tok_type)
        {
        case INT_LTRL:
            temp = Property_Int(NULL, atoi(curr_token_txt));
            break;
        case FLT_LTRL:
            temp = Property_Float(NULL, atof(curr_token_txt));
            break;
        case STRBODY:
            temp = Property_String(NULL, curr_token_txt);
            break;
        case NULL_LTRL:
        default:
            break;
        }
    }
    else if (relation == TO_ARR)  // handle array items
    {
        switch (tok_type)
        {
        case INT_LTRL:
            temp = ArrayItem_Int(atoi(curr_token_txt));
            break;
        case FLT_LTRL:
            temp = ArrayItem_Float(atof(curr_token_txt));
            break;
        case STRBODY:
            temp = ArrayItem_String(curr_token_txt);
            break;
        case NULL_LTRL:
        default:
            break;
        }
    }
    else if (relation == TO_PROP)  // handle object properties
    {
        switch (tok_type)
        {
        case INT_LTRL:
            temp = Property_Int(optional_name, atoi(curr_token_txt));
            break;
        case FLT_LTRL:
            temp = Property_Float(optional_name, atof(curr_token_txt));
            break;
        case STRBODY:
            temp = Property_String(optional_name, curr_token_txt);
            break;
        case NULL_LTRL:
        default:
            break;
        }
    }

    return temp;
}

void *Parser_Parse_Arr(Parser *self)
{
    puts("Parse [...]"); // DEBUG
    self->tokvec_idx++;  // skip past 1st bracket...

    int skip = 0;        // do not push a remaining value to array on commas
    int needs_comma = 0;
    int completed = 0;
    Token *temp_tok_ref = NULL;
    Array *result = Array_Create();

    if (!result)
        return result;

    void *parsed_val_ref = NULL;

    while (!completed && self->err_code == NO_ERR)
    {
        // stop parser on token vector end
        if (Parser_AtEnd(self))
        {
            completed = 1;
            break;
        }

        // peek at current token
        temp_tok_ref = TokenVec_At(self->tokvec_ref, self->tokvec_idx);

        switch (temp_tok_ref->type)
        {
        case LBRACKET:
            parsed_val_ref = ArrayItem_Chunk(Parser_Parse_Arr(self), ARR);
            needs_comma = 1;
            break;
        case LCURLY:
            parsed_val_ref = ArrayItem_Chunk(Parser_Parse_Obj(self), OBJ);
            needs_comma = 1;
            break;
        case INT_LTRL:
            parsed_val_ref = Parser_Parse_Prim(self, NULL, INT, TO_ARR);
            needs_comma = 1;
            break;
        case FLT_LTRL:
            parsed_val_ref = Parser_Parse_Prim(self, NULL, FLT, TO_ARR);
            needs_comma = 1;
            break;
        case NULL_LTRL:
            parsed_val_ref = Parser_Parse_Prim(self, NULL,  NUL, TO_ARR);
            needs_comma = 1;
            break;
        case STRBODY:
            parsed_val_ref = Parser_Parse_Prim(self, NULL, STR, TO_ARR);
            needs_comma = 1;
            break;
        case COMMA:
            if (!needs_comma)
            {
                self->err_code = UNEXPECTED_TOKEN_ERR;
                completed = 1;
            }
            else
                needs_comma = 0;
            break;
        default:
            break;  // ignore UNKNOWN tokens for now!
        }
        
        skip = !needs_comma && (temp_tok_ref->type == COMMA || temp_tok_ref->type == UNKNOWN);
        
        if (!skip && parsed_val_ref != NULL)
            Array_Push((Array*)result, (ArrayItem*)parsed_val_ref);

        self->tokvec_idx++;

        completed = temp_tok_ref->type == RBRACKET;
    }

    return result;
}

void *Parser_Parse_Obj(Parser *self)
{
    puts("Parse {...}");  // DEBUG
    self->tokvec_idx++;   // skip past 1st left curly brace

    int completed = 0;    // if current object is fully parsed
    int needs_attr = 1;   // expect attr. name
    int needs_colon = 0;  // expect colon
    int needs_value = 0;  // expect attr. value
    int needs_comma = 0;  // expect comma
    int skip_put = 0;     // whether to skip binding value to attr. (on commas)

    Token *curr_tok_ref = NULL;
    char *temp_attr_name = NULL;
    Property *todo_property = NULL;
    Object *result = NULL;

    size_t obj_buckets = 0;
    size_t relative_nest_lvl = 0;
    size_t last_tok_pos = self->tokvec_idx;

    while (!Parser_AtEnd(self)) {
        curr_tok_ref = TokenVec_At(self->tokvec_ref, self->tokvec_idx);  // prescan property count on this level to find out buckets

        switch(curr_tok_ref->type)
        {
        case LCURLY:
            relative_nest_lvl++;
            break;
        case RCURLY:
            relative_nest_lvl--;
        case COLON:
            if (relative_nest_lvl == 0)
                obj_buckets++;
            break;
        default:
            break;
        }

        self->tokvec_idx++;
    }

    // backtrack to first token of object
    self->tokvec_idx = last_tok_pos;

    result = Object_Create(obj_buckets);
    
    if (!result)
        return result;

    while (!completed && self->err_code == NO_ERR)
    {
        DataType parsed_value_type = UNSUPPORTED; // whether a token value is currently valid to bind to some attr.

        // stop parser on end of token vector
        if (Parser_AtEnd(self))
        {
            completed = 1;
            break;
        }

        // get current token
        curr_tok_ref = TokenVec_At(self->tokvec_ref, self->tokvec_idx);

        // check token to see what to parse
        switch (curr_tok_ref->type)
        {
        case STRBODY:
            if (needs_attr)
            {
                // puts("read attr"); // DEBUG
                temp_attr_name = Token_ToTxt(curr_tok_ref, self->srcbuf_ref);
                needs_attr = 0;
                needs_colon = 1;
            }
            else if (needs_value)
            {
                // puts("bind string");
                parsed_value_type = STR;
                todo_property = Property_String(temp_attr_name, Token_ToTxt(curr_tok_ref, self->srcbuf_ref));
                needs_value = 0;
                needs_comma = 1;
            }
            else
            {
                self->err_code = UNEXPECTED_TOKEN_ERR;
                completed = 1;
            }
            break;
        case COLON:
            if (!needs_colon)
            {
                self->err_code = UNEXPECTED_TOKEN_ERR;
                completed = 1;
            }
            else
            {
                needs_colon = 0;
                needs_value = 1;
            }
            break;
        case INT_LTRL:
            if (needs_value)
            {
                // puts("bind int"); // DEBUG
                parsed_value_type = INT;
                char *temp = Token_ToTxt(curr_tok_ref, self->srcbuf_ref);
                int value = 0;

                if (temp != NULL)
                {
                    value = atoi(temp);
                    todo_property = Property_Int(temp_attr_name, value);
                    free(temp);
                }

                needs_value = 0;
                needs_comma = 1;
            }
            else
            {
                self->err_code = UNEXPECTED_TOKEN_ERR;
                completed = 1;
            }
            break;
        case FLT_LTRL:
            if (needs_value)
            {
                // puts("bind float"); // DEBUG
                parsed_value_type = FLT;
                char *temp = Token_ToTxt(curr_tok_ref, self->srcbuf_ref);
                float value = 0.0f;

                if (temp != NULL)
                {
                    value = atof(temp);
                    parsed_value_type = INT;
                    todo_property = Property_Float(temp_attr_name, value);
                    free(temp);
                }
                
                needs_value = 0;
                needs_comma = 1;
            }
            else
            {
                self->err_code = UNEXPECTED_TOKEN_ERR;
                completed = 1;
            }
            break;
        case LCURLY:
            if (needs_value)
            {
                // puts("put {}"); // DEBUG
                parsed_value_type = OBJ;
                todo_property = Property_Chunk(temp_attr_name, Parser_Parse_Obj(self), parsed_value_type);
                needs_value = 0;
                needs_comma = 1;
            }
            else
            {
                self->err_code = UNEXPECTED_TOKEN_ERR;
                completed = 1;
            }
            break;
        case LBRACKET:
            if (needs_value)
            {
                //puts("put []"); // DEBUG
                parsed_value_type = ARR;
                todo_property = Property_Chunk(temp_attr_name, Parser_Parse_Arr(self), parsed_value_type);
                needs_value = 0;
                needs_comma = 1;
            }
            else
            {
                self->err_code = UNEXPECTED_TOKEN_ERR;
                completed = 1;
            }
            break;
        case COMMA:
            if (!needs_comma)
            {
                self->err_code = UNEXPECTED_TOKEN_ERR;
                completed = 1;
            }
            else
            {
                needs_attr = 1;
                needs_colon = 0;
                needs_value = 0;
                needs_comma = 0;
            }
            break;
        default:
            break;
        }

        skip_put = curr_tok_ref->type == COMMA || curr_tok_ref->type == COLON || curr_tok_ref->type == UNKNOWN || parsed_value_type == UNSUPPORTED || (curr_tok_ref->type == STRBODY && needs_attr);

        // bind property to parsing object when it's appropriate!
        if (!skip_put && todo_property != NULL)
            Object_SetItem(result, temp_attr_name, todo_property);
        
        self->tokvec_idx++;

        completed = curr_tok_ref->type == RCURLY;
    }

    return result;
}

int Parser_Get_ErrCode(const Parser *self) { return self->err_code; }

JsonThing *Parser_Start_Parse(Parser *self)
{
    if (!Parser_IsReady(self))
        return NULL;

    Token *temp_token_ref = NULL;
    DataType temp_root_type = UNSUPPORTED;
    JsonThing *result = NULL;

    // count colons on 1st nesting level (just the 1st layer of properties)...
    int prescan_nest_level = 0;
    size_t colon_count = 0;

    while (!Parser_AtEnd(self))
    {
        temp_token_ref = TokenVec_At(self->tokvec_ref, self->tokvec_idx);
        
        switch (temp_token_ref->type)
        {
        case LCURLY:
        case LBRACKET:
            prescan_nest_level++;
            break;
        case RCURLY:
        case RBRACKET:
            prescan_nest_level--;
            break;
        default:
            if (temp_token_ref->type == COLON && prescan_nest_level == 1)
                colon_count++;  // when I find an outermost property colon, count it!
            break;
        }

        self->tokvec_idx++;
    }

    self->tokvec_idx = 0;

    // reject unbalanced json!
    if (prescan_nest_level != 0)
    {
        self->err_code = UNBALANCED_NEST;
        return result;
    }

    temp_token_ref = TokenVec_At(self->tokvec_ref, self->tokvec_idx);

    switch (temp_token_ref->type)
    {
    case LCURLY:
        if (!self->temp_root)
        {
            self->temp_root = Property_Chunk(NULL, Parser_Parse_Obj(self), OBJ);
            temp_root_type = OBJ;
        }
        break;
    case LBRACKET:
        if (!self->temp_root)
        {
            self->temp_root = Property_Chunk(NULL, Parser_Parse_Arr(self), ARR);
            temp_root_type = ARR;
        }
        break;
    case INT_LTRL:
        if (!self->temp_root)
        {
            self->temp_root = Parser_Parse_Prim(self, NULL, INT, TO_NONE);
            temp_root_type = INT;
        }
        break;
    case FLT_LTRL:
        if (!self->temp_root)
        {
            self->temp_root = Parser_Parse_Prim(self, NULL, FLT, TO_NONE);
            temp_root_type = FLT;
        }
        break;
    case STRBODY:
        if (!self->temp_root)
        {
            self->temp_root = Parser_Parse_Prim(self, NULL, STR, TO_NONE);
            temp_root_type = STR;
        }
        break;
    default:
        break;
    }

    // reject invalid root json values!
    if (temp_root_type == UNSUPPORTED)
        return result;

    result = JsonThing_Create(temp_root_type, (Property*)self->temp_root);
    self->temp_root = NULL; // NOTE: now I can unbind old ref. ptr. to JSON root value!

    return result;
}
