#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "lexer.h"

#define MAX_CODE_LEN 4096

int lexer_debug_flag = 0;

//Table to look up correct token type
// More useful if we wanted to add more usable functions/operators
Token token_table[] = {{add, "+", 1},
                       {sub, "-", 1},
                       {mul, "*", 1},
                       {divide, "/", 1},
                       {mod, "%", 1},
                       {power, "^", 1},
                       {absolute, "abs", 3},
                       {number, "0", 0},
                       {end, "", 0},
                       {left_paren, "(", 1},
                       {right_paren, ")", 1},
                       {unknown, " ", 1}};

int token_table_size = sizeof(token_table) / sizeof(Token);

struct lexer
{
  char source_code[4096];
  char *cp;
  Token *cur_token;
};

Lexer *lexer_new(char *src)
{
  Lexer *new_lexer = malloc(sizeof(Lexer));
  strcpy(new_lexer->source_code, src);
  //Need to set to a pointer in order for advance token to not segfault
  new_lexer->cur_token = &token_table[unknown];
  new_lexer->cp = new_lexer->source_code;

  return new_lexer;
}

void lexer_free(Lexer *lexer)
{
  free(lexer);
}

Token *lexer_get_token(Lexer *lexer)
{
  return lexer->cur_token;
}

void read_number(Lexer *lexer, int *total_len, bool negative)
{
  int num_len = 0;
  // read in the negative sign if present
  if (negative)
  {
    num_len++;
    (lexer->cp)++;
  }

  while (*(lexer->cp) && isdigit(*(lexer->cp)))
  {
    num_len++;
    (lexer->cp)++;
  }
  strncpy(lexer->cur_token->buf, lexer->cp - num_len, num_len);
  lexer->cur_token->buf[num_len] = '\0';
  *total_len += num_len;
}

void lexer_advance_token(Lexer *lexer)
{
  if (lexer_debug_flag)
    fprintf(stderr, "[LEXER] Advancing token... \n");
  int total_len = 0;
  bool is_negative = false;
  Token *cur_token = lexer->cur_token;
  while (*(lexer->cp) && isspace(*(lexer->cp)))
  {
    ++(lexer->cp);
    ++total_len;
  }
  // Number is one special case
  if (isdigit(*(lexer->cp)))
  {
    lexer->cur_token = &token_table[number];
    read_number(lexer, &total_len, false);
    if (lexer_debug_flag)
      printf("[LEXER] Found number: %s\n", lexer->cur_token->buf);
    lexer->cur_token->total_len = total_len;
    return;
  }
  char buf[MAX_TOKEN_LEN] = "";

  // accumulate function name
  if (isalpha(*(lexer->cp)))
  {
    int word_len = 0;
    // read in the negative sign if present

    while (*(lexer->cp) && isalpha(*(lexer->cp)))
    {
      ++word_len;
      ++(lexer->cp);
    }
    strncpy(buf, lexer->cp - word_len, word_len);
    lexer->cur_token->buf[word_len] = '\0';
    total_len += word_len;
  }
  // If not a function assume its an operator that takes one char
  // Not the best but works in our simple case, not easily expandable to multichar operators
  else if (ispunct(*(lexer->cp)))
  {
    strncpy(buf, lexer->cp, 1);
    buf[1] = '\0';

    ++(lexer->cp);
  }
  bool found = false;
  if (lexer_debug_flag)
    printf("[LEXER] Searching for %s\n", buf);
  for (int ix = 0; ix < token_table_size; ix++)
  {
    if (!strcmp(buf, token_table[ix].buf))
    {
      found = true;
      TokenType prev_type = lexer->cur_token->type;
      lexer->cur_token = &token_table[ix];
      // need to check if prev token was a right paren or number to handle the following cases:
      //      (1 + 2)-2 and 4-2
      // If only we only use the fact that the following char is a digit we error on cases where there was just no space
      if (token_table[ix].type == sub && isdigit(*lexer->cp) && prev_type != right_paren && prev_type != number)
      {
        lexer->cp--;
        lexer->cur_token = &token_table[number];
        read_number(lexer, &total_len, true);
      }

      break;
    }
  }

  if (!found)
  {
    lexer->cur_token = &token_table[unknown];
  }

  lexer->cur_token->total_len = total_len;
  if (lexer_debug_flag)
    fprintf(stderr, "[LEXER] Token Found: '%s', type %d, len %d\n", lexer->cur_token->buf,
            lexer->cur_token->type, lexer->cur_token->total_len);
}
