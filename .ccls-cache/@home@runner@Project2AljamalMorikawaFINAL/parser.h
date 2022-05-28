#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "lexer.h"

enum parser_errors
{
  VALID,
  INVALID_EXPRESSION,
  MISSING_OPERAND,
  MISSING_OPERATOR
};

/*
  THE LANGUAGE WE'RE IMPLEMENTING (Look up 'EBNF')

expression ::= term ( ('+'|'-') term )*
term ::= exp ( ('*' | '/' | '%) exp )*
exp ::= factor ( '^' exp)?
factor ::= '(' expression ')' | NUMBER | 'abs' factor
NUMBER ::= '-'? DIGIT+
DIGIT ::= '0' | '1' | '2'….
*/

struct parser;
typedef struct parser Parser;

extern int parser_debug_flag;

/**
 * @brief Creates a Parser object.
 *
 * @param buf The string to parse.
 * @return Parser* The new parser object.
 */
Parser *parser_new(char *buf);

/**
 * @brief Releases the resources used by the given parser.
 */
void parser_free(Parser *parser);

/**
 * @brief A parser used to parse an infix string.
 *
 * @return A bool indicating if it could be parsed.
 */
bool parser_parse_infix(Parser *parser);

/**
 * @brief Evalutes each given instruction.
 *
 * @return long int The result of all the operations.
 */
long int parser_evaluate(Parser *parser, int *errno);

/**
 * @brief A parser used to parse a postfix string.
 *
 * @return A bool indicating if it could be parsed.
 */
bool parser_parse_postfix(Parser *parser);

/**
 * @brief Prints all the postifx operations.
 */
void parser_output_postfix(Parser *parser);

/**
 * @brief Enables debugging mode for detailed messages.
 */
void parser_debug_mode();

#endif