#include "parser.h"
#include "stack.h"
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#define MAXLEN 4096
#define IGNORE_VALUE 0
int parser_debug_flag = 0;

typedef struct instruction {
  TokenType opcode;
  long int value;
} Instruction;

struct parser {
  Lexer *lexer;
  Instruction compiled[MAXLEN];
  int compiled_len;
};

// Return false if operation is unsucessful
typedef bool (*StackOperationFunc)(Stack *stack, long int value, int *error);
char *tokens_as_strings[] = {"+ ", "- ", "* ", "/ ", "% ", "^ ", "abs ", ""};
char *tokens_by_name[] = {"ADD", "SUB", "MUL", "DIV",
                          "MOD", "POW", "ABS", "NONE"};

bool stackop_add(Stack *stack, long int value, int *error);
bool stackop_sub(Stack *stack, long int value, int *error);
bool stackop_mul(Stack *stack, long int value, int *error);
bool stackop_div(Stack *stack, long int value, int *error);
bool stackop_mod(Stack *stack, long int value, int *error);
bool stackop_pow(Stack *stack, long int value, int *error);
bool stackop_abs(Stack *stack, long int value, int *error);
bool stackop_push_num(Stack *stack, long int value, int *error);

// Testing 1 more thing
bool calculate_internal(Stack **stack, TokenType type, int *error);

// Position is same as the TokenType enum that we also use for opcodes
// Used in the evaluator
StackOperationFunc stack_operation_table[] = {
    stackop_add, stackop_sub, stackop_mul, stackop_div,
    stackop_mod, stackop_pow, stackop_abs, stackop_push_num};

Parser *parser_new(char *buf) {
  Parser *new_parser = malloc(sizeof(Parser));
  new_parser->lexer = lexer_new(buf);
  new_parser->compiled_len = 0;

  return new_parser;
}

void parser_free(Parser *parser) {
  lexer_free(parser->lexer);
  free(parser);
}

void emit(Parser *parser, TokenType opcode, int val) {
  parser->compiled[parser->compiled_len].opcode = opcode;
  parser->compiled[parser->compiled_len].value = val;

  ++(parser->compiled_len);
}

bool p_expression(Parser *parser);
bool p_term(Parser *parser);
bool p_exp(Parser *parser);
bool p_factor(Parser *parser);
bool p_group(Parser *parser);

bool parser_parse_infix(Parser *parser) {

  lexer_advance_token(parser->lexer);
  bool valid = p_expression(parser);
  if (lexer_get_token(parser->lexer)->type != end) {
    valid = false;
  }

  return valid;
}

bool p_expression(Parser *parser) {
  if (parser_debug_flag) {
    fprintf(stderr, "[PARSER] expression ::= term ( ('+'|'-') term )*\n");
  }
  bool valid = p_term(parser);
  Token *tok = lexer_get_token(parser->lexer);
  if (!valid) {
    return valid;
  }
  while (valid && (tok->type == add || tok->type == sub)) {
    TokenType opcode = tok->type;
    lexer_advance_token(parser->lexer);
    valid = p_term(parser);
    if (valid) {
      emit(parser, opcode, IGNORE_VALUE);
    }
    //update token for while loop
    tok = lexer_get_token(parser->lexer);
  }

  return valid;
}

bool p_term(Parser *parser) {
  if (parser_debug_flag)
    fprintf(stderr, "[PARSER] term ::= exp ( ('*' | '/' | '%%') exp )*\n");

  bool valid = p_exp(parser);
  Token *tok = lexer_get_token(parser->lexer);
  if (!valid) {
    return valid;
  }
  while (valid &&
         (tok->type == mul || tok->type == divide || tok->type == mod)) {
    TokenType opcode = tok->type;
    lexer_advance_token(parser->lexer);
    valid = p_exp(parser);
    if (valid) {
      emit(parser, opcode, IGNORE_VALUE);
    }
    //update token for while loop
    tok = lexer_get_token(parser->lexer);
  }

  return valid;
}

bool p_exp(Parser *parser) {
  if (parser_debug_flag)
    fprintf(stderr, "[PARSER] exp ::= factor ( '^' exp)?\n");

  bool valid = p_factor(parser);
  Token *tok = lexer_get_token(parser->lexer);
  if (!valid) {
    return valid;
  }
  if (valid && tok->type == power) {
    TokenType opcode = tok->type;
    lexer_advance_token(parser->lexer);
    valid = p_exp(parser);
    if (valid) {
      emit(parser, opcode, IGNORE_VALUE);
    }
  }
  return valid;
}

bool p_factor(Parser *parser) {
  if (parser_debug_flag)
    fprintf(stderr, "[PARSER] factor ::= '(' expression ')' | NUMBER | "
                    "'abs''(' expression ')'\n");

  Token *tok = lexer_get_token(parser->lexer);
  bool valid = true;
  if (tok->type == left_paren) {
    lexer_advance_token(parser->lexer);
    valid = p_expression(parser);
    tok = lexer_get_token(parser->lexer);
    if (tok->type == right_paren) {
      lexer_advance_token(parser->lexer);
    } else {
      valid = false;
    }
  } else if (tok->type == number) {
    if (parser_debug_flag)
      fprintf(stderr, "[PARSER] Number Found: %s\n", tok->buf);
    emit(parser, number, atol(tok->buf));
    lexer_advance_token(parser->lexer);
  }else if(tok->type == absolute){
    TokenType opcode = tok->type;
    lexer_advance_token(parser->lexer);
    valid = p_factor(parser);
    if(!valid)
      return valid;
    emit(parser, opcode, IGNORE_VALUE);
  } else {
    valid = false;
  }
  return valid;
}

long int parser_evaluate(Parser *parser, int *error) {
  long int v1, v2;
  Stack *stack = stack_create();
  Instruction instruction;
  int err = 0;

  for (int ix = 0; ix < parser->compiled_len; ix++) {
    instruction = parser->compiled[ix];
    TokenType opcode = parser->compiled[ix].opcode;
    StackOperationFunc stack_func = stack_operation_table[opcode];
    // make sure that the value is within the range of array
    // Mainly as a precaution
    if (opcode < add || opcode > number) {
      stack_free(stack);
      *error = INVALID_EXPRESSION;
      return 0;
    }
    if (!stack_func(stack, parser->compiled[ix].value, error)) {
      stack_free(stack);
      return 0;
    }
    if (parser_debug_flag) {
      fprintf(stderr, "[EVALUATOR] Instruction: %s, value: %ld\n",
              tokens_by_name[instruction.opcode], instruction.value);
      fprintf(stderr, "Stack:\n");
      stack_print(stack);
    }
  }

  long int result = stack_pop(stack, &err);
  if (err || !stack_empty(stack)) {
    stack_free(stack);
    *error = MISSING_OPERATOR;
    return 0;
  }
  stack_free(stack);
  return result;
}

void parser_output_postfix(Parser *parser) {
  printf("Postfix: ");
  for (int ix = 0; ix < parser->compiled_len; ix++) {
    Instruction instruction = parser->compiled[ix];
    if (instruction.opcode == number) {
      printf("%ld ", instruction.value);
    } else {
      printf("%s", tokens_as_strings[instruction.opcode]);
    }
  }
  printf("\n");
}

bool parser_parse_postfix(Parser *parser) {
  Token *tok;
  lexer_advance_token(parser->lexer);
  tok = lexer_get_token(parser->lexer);
  while (tok->type != end) {
    TokenType opcode = tok->type;
    if (opcode == unknown) {
      return false;
    }
    if (opcode == number)
      emit(parser, opcode, atoll(tok->buf));
    else
      emit(parser, opcode, IGNORE_VALUE);

    lexer_advance_token(parser->lexer);
    tok = lexer_get_token(parser->lexer);
  }

  // Assume true handle error from this in evaluator
  return true;
}

void parser_debug_mode() {
  lexer_debug_flag = 1;
  parser_debug_flag = 1;
}

bool stackop_add(Stack *stack, long int value, int *error) {
  long int v1, v2;
  int err = 0;
  v1 = stack_pop(stack, &err);
  v2 = stack_pop(stack, &err);
  if (err) {
    *error = MISSING_OPERAND;
    return false;
  }
  if (parser_debug_flag) {
    fprintf(stderr, "[EVALUATOR] %ld + %ld\n", v2, v1);
  }
  stack_push(stack, v2 + v1);
  return true;
}

bool stackop_sub(Stack *stack, long int value, int *error) {
  long int v1, v2;
  int err = 0;
  v1 = stack_pop(stack, &err);
  v2 = stack_pop(stack, &err);
  if (err) {
    *error = MISSING_OPERAND;
    return false;
  }
  if (parser_debug_flag) {
    fprintf(stderr, "[EVALUATOR] %ld - %ld\n", v2, v1);
  }
  stack_push(stack, v2 - v1);
  return true;
}

bool stackop_mul(Stack *stack, long int value, int *error) {
  long int v1, v2;
  int err = 0;
  v1 = stack_pop(stack, &err);
  v2 = stack_pop(stack, &err);
  if (err) {
    *error = MISSING_OPERAND;
    return false;
  }
  if (parser_debug_flag) {
    fprintf(stderr, "[EVALUATOR] %ld * %ld\n", v2, v1);
  }
  stack_push(stack, v2 * v1);
  return true;
}

bool stackop_div(Stack *stack, long int value, int *error) {
  long int v1, v2;
  int err = 0;
  v1 = stack_pop(stack, &err);
  v2 = stack_pop(stack, &err);
  if (err) {
    *error = MISSING_OPERAND;
    return false;
  }
  if (parser_debug_flag) {
    fprintf(stderr, "[EVALUATOR] %ld / %ld\n", v2, v1);
  }
  stack_push(stack, v2 / v1);
  return true;
}

bool stackop_mod(Stack *stack, long int value, int *error) {
  long int v1, v2;
  int err = 0;
  v1 = stack_pop(stack, &err);
  v2 = stack_pop(stack, &err);
  if (err) {
    *error = MISSING_OPERAND;
    return false;
  }
  if (parser_debug_flag) {
    fprintf(stderr, "[EVALUATOR] %ld %% %ld\n", v2, v1);
  }
  stack_push(stack, v2 % v1);
  return true;
}

bool stackop_pow(Stack *stack, long int value, int *error) {
  long int v1, v2;
  int err = 0;
  v1 = stack_pop(stack, &err);
  v2 = stack_pop(stack, &err);
  if (err) {
    *error = MISSING_OPERAND;
    return false;
  }
  if (parser_debug_flag) {
    fprintf(stderr, "[EVALUATOR] pow(%ld , %ld)\n", v2, v1);
  }
  stack_push(stack, (long int)pow(v2, v1));
  return true;
}

bool stackop_abs(Stack *stack, long int value, int *error) {
  long int v1;
  int err = 0;
  v1 = stack_pop(stack, &err);
  if (err) {
    *error = MISSING_OPERAND;
    return false;
  }
  if (parser_debug_flag) {
    fprintf(stderr, "[EVALUATOR] Absoule value of %ld \n", v1);
  }

  stack_push(stack, labs(v1));
  return true;
}

bool stackop_push_num(Stack *stack, long int value, int *error) {
  if (parser_debug_flag) {
    fprintf(stderr, "[EVALUATOR] Pushing %ld \n", value);
  }
  stack_push(stack, value);
  return true;
}
