/**
 * @file json_lex.c
 * @author Derek Tan (DrkWithT at Github)
 * @brief Implements my JSON lexer.
 * @date 2023-03-26
 */

#include "json_lex.h"

char *read_file(const char *file_path, size_t *external_len)
{
    FILE *fs = fopen(file_path, "r");
    char *buf = NULL;
    size_t buf_size = 0;
    size_t buf_pos = 0;

    if (!fs)
        goto err_bail; // error case 1: no file

    // get file length
    fseek(fs, 0, SEEK_END);
    buf_size = ftell(fs) + 1; // +1 for nul terminator
    fseek(fs, 0, SEEK_SET);

    if (buf_size == 0 || buf_size > MAX_JSON_LEN)
        goto err_bail; // error case 2: file too big or small

    buf = malloc(sizeof(char) * buf_size);

    if (!buf)
        goto err_bail; // error case 3: no buffer

    while (buf_pos < buf_size)
    {
        buf[buf_pos] = fgetc(fs);
        buf_pos++;
    }

    buf[buf_pos] = '\0';
    *external_len = buf_size - 1;

err_bail: // cleanup on bad or normal exit
    if (fs != NULL)
        fclose(fs);
    
    return buf;
}

Lexer *Lexer_Create(const char *file_name)
{
    Lexer *result = malloc(sizeof(Lexer));

    if (!result)
        return result;
    
    size_t temp_doc_len = 0;
    result->doc_buf = read_file(file_name, &temp_doc_len);
    result->doc_end = temp_doc_len;
    result->doc_pos = 0;

    // hacky way to avoid extra allocs, but oh well!
    result->special_null[0] = 'n';
    result->special_null[1] = 'u';
    result->special_null[2] = 'l';
    result->special_null[3] = 'l';
    result->special_null[4] = '\0';

    return result;
}

char *Lexer_CleanUp(Lexer *self)
{
    char *temp = self->doc_buf; // copy referencing addr. to avoid losing the buffer!
    self->doc_buf = NULL;

    self->doc_end = 0;
    self->doc_pos = 0;

    return temp;
}

void Lexer_Skip_WSpc(Lexer *self)
{
    char c = '\0';

    do
    {
        c = self->doc_buf[self->doc_pos];

        if (!is_wspace(c))
            break;
        
        self->doc_pos++;
    } while (self->doc_pos < self->doc_end);
}

Token *Lexer_Lex_Punct(Lexer *self, TokenType punct_kind)
{
    size_t temp_start = self->doc_pos;
    self->doc_pos++;

    return Token_Create(punct_kind, temp_start, 1);
}

Token *Lexer_Lex_Str(Lexer *self)
{
    size_t curr_start = self->doc_pos;
    size_t curr_span = 0;
    char c = '\0';

    do
    {
        c = self->doc_buf[self->doc_pos];

        if (match_punct(c, '\"'))
        {
            self->doc_pos++; // skip past end quote to avoid stalling lexer loop!
            break;
        }

        curr_span++;
        self->doc_pos++;
    } while (self->doc_pos < self->doc_end);

    return Token_Create(STRBODY, curr_start, curr_span);
}

Token *Lexer_Lex_Num(Lexer *self)
{
    int digit_count = 0; // to check against lone points!
    int point_count = 0; // if > 1, then the literal is NaN!
    size_t curr_start = self->doc_pos;
    size_t curr_span = 0;
    char c = '\0';

    do
    {
        c = self->doc_buf[self->doc_pos];

        if (!is_digit(c) && !match_punct(c, '.'))
            break;
        else if (match_punct(c, '.'))
            point_count++;
        else if (is_digit(c))
            digit_count++;

        // reject invalid decimals
        if (point_count > 1)
            break;

        curr_span++;
        self->doc_pos++;
    } while (self->doc_pos < self->doc_end);

    if (point_count == 0)
        return Token_Create(INT_LTRL, curr_start, curr_span);
    else if (point_count == 1)
        return Token_Create(FLT_LTRL, curr_start, curr_span);
    
    return Token_Create(UNKNOWN, curr_start, curr_span);
}

Token *Lexer_Lex_Null(Lexer *self)
{
    int valid_null = 1;
    size_t curr_start = self->doc_pos;
    size_t curr_span = 1;
    int check_idx = 0; // index of lexer's "null" string
    char c = '\0';

    do
    {
        c = self->doc_buf[self->doc_pos];

        // track current token text against "null"
        if (self->special_null[check_idx] == c)
            check_idx++;
        else
        {
            valid_null = 0;
            break;
        }

        curr_span++;

        // handle case of full "null" text
        if (check_idx == 3)
            break;

        self->doc_pos++;
    }
    while (self->doc_pos < self->doc_end);

    self->doc_pos++;

    if (!valid_null)
        return Token_Create(UNKNOWN, curr_start, curr_span);
    
    return Token_Create(NULL_LTRL, curr_start, curr_span);
}

TokenVec *Lexer_Lex_All(Lexer *self)
{
    size_t result_idx = 0; // token vector insert position
    Token *temp = NULL;
    TokenVec *result = TokenVec_Create(8); // collection of lexed tokens
    char peeked_char = '\0';

    while (1)
    {
        // check for EOF before consuming any other token!
        if (self->doc_pos >= self->doc_end)
        {
            temp = Token_Create(FILE_END, self->doc_pos, 1);
            break;   
        }

        peeked_char = self->doc_buf[self->doc_pos];

        switch (peeked_char)
        {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            Lexer_Skip_WSpc(self);
            break;
        case '[':
            temp = Lexer_Lex_Punct(self, LBRACKET);
            break;
        case ']':
            temp = Lexer_Lex_Punct(self, RBRACKET);
            break;
        case '{':
            temp = Lexer_Lex_Punct(self, LCURLY);
            break;
        case '}':
            temp = Lexer_Lex_Punct(self, RCURLY);
            break;
        case '\"':
            self->doc_pos++;
            temp = Lexer_Lex_Str(self);
            break;
        case ':':
            temp = Lexer_Lex_Punct(self, COLON);
            break;
        case 'n':
            temp = Lexer_Lex_Null(self);
        case ',':
            temp = Lexer_Lex_Punct(self, COMMA);
            break;
        default:
            if (is_digit(peeked_char))
                temp = Lexer_Lex_Num(self);
            else
                temp = Token_Create(UNKNOWN, self->doc_pos, 1);
            break;
        }

        // append new token in order!
        TokenVec_Set(result, result_idx, temp);
    }
    
    return result;
}
