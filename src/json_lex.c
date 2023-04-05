/**
 * @file json_lex.c
 * @author Derek Tan (DrkWithT at Github)
 * @brief Implements my JSON lexer.
 * @date 2023-03-26
 */

#include "json_lex.h"

int is_wspace(char c) { return c == ' ' || c == '\t' || c == '\n' || c =='\r'; }
int is_digit(char c) { return c >= '0' && c <= '9'; }

char *read_file(const char *file_path, size_t *external_len)
{
    FILE *fs = fopen(file_path, "r");
    char *buf = NULL;
    size_t buf_size = 0;
    size_t buf_pos = 0;

    if (!fs)
        goto err_bail; // error case 1: no file

    fseek(fs, 0, SEEK_END);
    buf_size = ftell(fs) + 1; // +1 for nul terminator
    fseek(fs, 0, SEEK_SET);

    if (buf_size == 0 || buf_size > MAX_JSON_LEN)
        goto err_bail; // error case 2: file too big or small

    buf = malloc(sizeof(char) * buf_size);

    if (!buf)
        goto err_bail; // error case 3: no buffer

    fread(buf, sizeof(char), buf_size - 1, fs);

    buf[buf_size - 1] = '\0';
    *external_len = buf_size - 1;

err_bail: // cleanup on any exit
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

    strcpy(result->special_null, "null");
    result->special_null[4] = '\0'; // put null terminator to avoid over-reading

    return result;
}

char *Lexer_CleanUp(Lexer *self)
{
    char *temp = self->doc_buf; // Get referencing addr. to avoid losing the buffer meant for stringifying tokens in the parser.
    self->doc_buf = NULL;

    self->doc_end = 0;
    self->doc_pos = 0;

    return temp;
}

int Lexer_CanUse(const Lexer *self)
{
    if (self == NULL)
        return 0;

    return self->doc_buf != NULL;
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
    self->doc_pos++;

    size_t curr_start = self->doc_pos;
    size_t curr_span = 0;
    char c = '\0';
    
    do
    {
        c = self->doc_buf[self->doc_pos];

        if (c == '\"')
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

        if (!is_digit(c) && c != '.')
            break;
        else if (c == '.')
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

        if (is_wspace(peeked_char))
        {
            Lexer_Skip_WSpc(self);
            continue;
        }

        switch (peeked_char)
        {
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

        TokenVec_Set(result, result_idx, temp); // append a new token to a dynamic vector

        result_idx++;
    }
    
    return result;
}
