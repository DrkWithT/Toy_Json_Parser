#ifndef JSON_LEX_H
#define JSON_LEX_H

#include "json_token.h"

/// Limits:
#define MAX_JSON_LEN 10000

/// Helpers:

int is_wspace(char c);
int is_digit(char c);
int match_punct(char c, char ch);

/**
 * @brief Reads an entire file (json) into a dynamic char buffer. Returns NULL on failure. Either failed allocation or a size too large (over MAX_JSON_LEN) will cause failure.
 * 
 * @param file_path The file path.
 * @return char*
 */
char *read_file(const char *file_path, size_t *external_len);

/// Lexer:

typedef struct json_lex
{
    char special_null[5]; // cached "null" name for lexing
    char *doc_buf;
    size_t doc_pos;
    size_t doc_end;
} Lexer;

/**
 * @brief Creates and initializes a new Lexer with an internal buffer of the small JSON file.
 * 
 * @param file_path Constant C-String naming the file.
 * @return Lexer*
 */
Lexer *Lexer_Create(const char *file_path);

/**
 * @brief Moves out the dynamic buffer (to the parser) after resetting Lexer data.
 * 
 * @param self
 */
char *Lexer_CleanUp(Lexer *self);

/**
 * @brief Checks if the lexer can lex. Requires the Lexer to be non-NULL and the buffer to be allocated.
 * 
 * @param self
 * @return int
 */
int Lexer_CanUse(const Lexer *self);

void Lexer_Skip_WSpc(Lexer *self);
Token *Lexer_Lex_Punct(Lexer *self, TokenType punct_kind);
Token *Lexer_Lex_Str(Lexer *self);
Token *Lexer_Lex_Num(Lexer *self);
Token *Lexer_Lex_Null(Lexer *self);
TokenVec *Lexer_Lex_All(Lexer *self);

#endif
