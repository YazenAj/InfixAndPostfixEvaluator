#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*
  THE LANGUAGE WE'RE IMPLEMENTING (Look up 'EBNF')

expression ::= term ( (‘+’|’-’) term )*
term ::= factor ( (‘*’ | ‘/’) factor )*
factor ::= ‘(‘ expression ‘)’ | NUMBER
NUMBER ::= ‘-’? DIGIT+
DIGIT ::= ‘0’ | ‘1’ | ‘2’….
*/

#define MAXLEN 4096
int debug_flag = 0;

// These are used as both parsing tokens and compiled opcodes. that's not good.
typedef enum token_type {
  add,
  sub,
  mul,
  div,
  number,
  end,
  left_paren,
  right_paren,
  unknown
} TokenType;
char *tokens_as_strings[] = {"PLUS", "MINUS", "TIMES", "DIVIDE", "PUSH"};

// This is compiled instruction type
typedef struct instruction {
  TokenType opcode;
  int value;
} Instruction;

Instruction compiled[MAXLEN];
int compiled_length;

// false for error, true for valid
bool parse_program(char *buf);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "The first argument must be the program to compile\n");
    return 1;
  }

  char source[MAXLEN];
  strcpy(source, argv[1]);

  bool valid = parse_program(source);
  printf("prog is %sa valid program\n", valid ? "" : "NOT ");

  if (valid) {
    printf("program:\n");
    for (int ix = 0; ix < compiled_length; ix++) {
      printf("%-7s ", tokens_as_strings[compiled[ix].opcode]);
      if (compiled[ix].opcode == number) {
        printf("%5d", compiled[ix].value);
      }
      putchar('\n');
    }
  }
  return 0;
}

void advance_token(void);
bool p_expression(void);
bool p_term(void);
bool p_factor(void);
void read_number(int *total_len, bool negative);

typedef struct token {
  TokenType type;
  char buf[10];
  int total_len; // length of token plus any leading spaces
} Token;

// program and "next" pointer are global for convenience
char prog_source[MAXLEN];
char *cp;
Token next_token;

// we just use this to indicate in the source that the opcode does not need a
// value attached
#define IGNORE_VALUE 0

// add an instruction to the compiled program
void emit(TokenType opcode, int val) {
  compiled[compiled_length].opcode = opcode;
  compiled[compiled_length].value = val;
  ++compiled_length;
}

bool parse_program(char *buf) {
  // we do this in case we want to modify the source, which we don't currently
  // do;
  strcpy(prog_source, buf);
  cp = prog_source;

  compiled_length = 0;

  advance_token();
  bool valid = p_expression();
  if (next_token.type != end)
    valid = false;

  return valid;
}

bool p_expression(void) {
  bool valid = p_term();
  if (!valid)
    return valid;
  while (valid && next_token.type == add || next_token.type == sub) {
    TokenType opcode = next_token.type;
    advance_token();
    valid = p_term();
    if (valid) {
      emit(opcode, IGNORE_VALUE);
    }
  }
  return valid;
}

bool p_term(void) {
  bool valid = p_factor();
  if (!valid)
    return valid;
  while (valid && next_token.type == mul || next_token.type == div) {
    TokenType opcode = next_token.type;
    advance_token();
    valid = p_factor();
    if (valid) {
      emit(opcode, IGNORE_VALUE);
    }
  }
  return valid;
}

bool p_factor(void) {
  bool valid = true;
  if (next_token.type == left_paren) {
    advance_token();
    valid = p_expression();
    if (next_token.type == right_paren) {
      advance_token();
    } else {
      valid = false;
    }
  } else if (next_token.type == number) {
    valid = true;
    emit(number, atoi(next_token.buf));
    advance_token();
  } else {
    valid = false;
  }
  return valid;
}

void advance_token(void) {
  int total_len = 0;
  bool is_negative = false;
  while (*cp && isspace(*cp)) {
    ++cp;
    ++total_len;
  }
  // this switch statement should be converted to data except for the number
  // parsing
  switch (*cp) {
  case '\0':
    next_token.type = end;
    strcpy(next_token.buf, "");
    break;
  case '+':
    next_token.type = add;
    strcpy(next_token.buf, "+");
    ++cp;
    ++total_len;
    break;
  case '-':
    if(isdigit(cp[1]) ){
        is_negative = true;
        next_token.type = number;
        read_number(&total_len, is_negative);
    }
    else{
      next_token.type = sub;
      strcpy(next_token.buf, "-");
      ++cp;
      ++total_len;
    }
    break;

    
  case '*':
    next_token.type = mul;
    strcpy(next_token.buf, "*");
    ++cp;
    ++total_len;
    break;
  case '/':
    next_token.type = div;
    strcpy(next_token.buf, "/");
    ++cp;
    ++total_len;
    break;
  case '(':
    next_token.type = left_paren;
    strcpy(next_token.buf, "(");
    ++cp;
    ++total_len;
    break;
  case ')':
    next_token.type = right_paren;
    strcpy(next_token.buf, ")");
    ++cp;
    ++total_len;
    break;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9': {
    next_token.type = number;
    read_number(&total_len, is_negative);

    break;
  }
  default:
    next_token.type = unknown;
    next_token.buf[0] = *cp++;
    next_token.buf[1] = ' ';
    total_len++;
    break;
  }
  next_token.total_len = total_len;
  if (debug_flag)
    fprintf(stderr, "Got token '%s', type %d, len %d\n", next_token.buf,
            next_token.type, next_token.total_len);
}

void read_number(int *total_len, bool negative){
  int num_len = 0;
  //read in the negative sign if present
  if(negative){
    num_len++;
    cp++;
  }

    while (*cp && isdigit(*cp)) {
      num_len++;
      cp++;
    }
    strncpy(next_token.buf, cp - num_len, num_len);
    next_token.buf[num_len] = '\0';
    *total_len += num_len;
}

