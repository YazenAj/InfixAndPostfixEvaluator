#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKEN_LEN 64

typedef enum token_type
{
  add,
  sub,
  mul,
  divide,
  mod,
  power,
  absolute,
  number,
  end,
  left_paren,
  right_paren,
  unknown
} TokenType;

struct lexer;
typedef struct lexer Lexer;

typedef struct token
{
  TokenType type;
  char buf[MAX_TOKEN_LEN];
  int total_len;
} Token;

extern int lexer_debug_flag;

/**
 * @brief Advances the lexer to the next token.
 */
void lexer_advance_token(Lexer *lexer);

/**
 * @brief Creates a Lexer object.
 *
 * @param src The string that must be analyzed
 * @return Lexer* The new Lexer object.
 */
Lexer *lexer_new(char *src);

/**
 * @brief Frees the resources of the given lexer.
 */
void lexer_free(Lexer *lexer);

/**
 * @brief Gets the current token.
 * @return The current lexer token.
 */
Token *lexer_get_token(Lexer *lexer);

#endif