

/*
  Infix -> Postfix converter and Postfix evaluator program by Michael Morikawa
  and Yazen Aljamal.

*****************************TEST CASES********************************** 
=============================INFIX TESTS =============================
--Expression------------------------ Expected Result---------Comment---------
"33"                                        33             // single operand
"4 + 4"                                     8              //Simple expression
"(4 + 9/2 - -8)^3 + abs(15 % 4 - 5*2)"      4103           //Tests all operators
"2^3^2"                                     512            //Assume right associativity
                                                             equivalent to 2^(3^2)
"(5+(10*(5+5)))"                            105            //Chained parenthesis 
""                                          error          //null input
"(1 + 2))"                                  error          //extra closing paren
"(( 4 + -4 + 9/3)*5"                        error          //missing closing paren
"abs()"                                     error          //no operand for abs
" 3 + -4 *"                                 error          //hanging operator

============================POSTFIX TESTS =============================
**Note: These are the expected postfix expression of the valid infix expressions**
--Expression------------------------ Expected Result---------Comment---------
"33"                                        33                  
"4 4 +"                                     8                 
"4 9 2 / + -8 - 3 ^ 15 4 % 5 2 * - abs +"   4103               
"2 3 2 ^ ^"                                 512                
"5 10 5 5 + * +"                            105            

""                                          error          //null input
"1 + 1"                                     error          //wrong order
"abs"                                       error          //missing operand
"1 %"                                       error          //missing operand
"4 5 3 * "                                  error          //missing operator
*****************************************************************************

Valid expressions generated using 

https://www.mathblog.dk/tools/infix-postfix-converter/

For absolute value simply omitted it from the input then added it to the correct
spot in the generated output
*/
#include "parser.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_BUF 1024
typedef bool (*ParseFunc)(Parser *parser);

void print_help();

int main(int argc, char *argv[]) {
  char source[MAX_BUF];
  bool output_postfix = false;
  bool c_input = false;
  bool sample = false;
  ParseFunc parse_func = parser_parse_infix;

  for (int ix = 1; ix < argc && !sample; ix++) {
    // check if '-' followed by digit to handle cases where expression starts
    // with a negative
    if (argv[ix][0] == '-' && !isdigit(argv[ix][1])) {
      switch (argv[ix][1]) {
      case 'h':
        print_help();
        return 0;
      case 's':
        printf("Testing all operations using: (4 + 9/2 - -8)^3 + abs(15 %% 4 - 5*2)\n");
        output_postfix = true;
        parser_debug_mode();
        parse_func = parser_parse_infix;
        strcpy(source, "(4 + 9/2 - -8)^3 + abs(15 % 4 - 5*2)");
        c_input = true;
        sample = true;
        break;
      case 'v':
        output_postfix = true;
        break;
      case 'r':
        parse_func = parser_parse_postfix;
        break;
      case 'd':
        parser_debug_mode();
        break;
      default:
        fprintf(stderr, "Unkown command %s\n", argv[ix]);
        return 1;
        break;
      }
    } else {
      strcpy(source, argv[ix]);
      c_input = true;
    }
  }

  if (!c_input) {
    fgets(source, MAX_BUF, stdin);
  }

  Parser *parser = parser_new(source);
  bool valid = parse_func(parser);
  int err = 0;
  if (valid) {
    long int res = parser_evaluate(parser, &err);
    if (err) {
      switch (err) {
      case INVALID_EXPRESSION:
        fprintf(stderr, "ERROR: Invalid Expression\n");
        break;
      case MISSING_OPERAND:
        fprintf(stderr, "ERROR: Missing Operand(s)\n");
        break;
      case MISSING_OPERATOR:
        fprintf(stderr, "ERROR: Missing Operator(s)\n");
        break;
      }

      parser_free(parser);
      return 1;
    } else {
      if (output_postfix)
        parser_output_postfix(parser);

      printf("Result: %ld\n", res);
    }

  } else {
    fprintf(stderr, "Invalid expression\n");
    parser_free(parser);
    return 1;
  }
  parser_free(parser);
  return 0;
}


void print_help()
{
  printf("************************************************\n"
         "************************************************\n"
         "**          Infix and Postfix Parser          **\n"
         "**--------------------------------------------**\n"
         "**                  Commands                  **\n"
         "**--------------------------------------------**\n"
         "** -h......................Displays Help Menu **\n"
         "** -d....................Toggles Debug Output **\n"
         "** -v..................Display Postfix Result **\n"
         "** -r....................Set input to PostFix **\n"
         "** -s......Tests all operations with a sample **\n"
         "**--------------------------------------------**\n"
         "**                  Operators                 **\n"
         "**--------------------------------------------**\n"
         "** Addition        ➜  +                       **\n"
         "** Subtraction     ➜  -                       **\n"
         "** Division        ➜  /                       **\n"
         "** Multiplication  ➜  *                       **\n"
         "** Modulo          ➜  %%                       **\n"
         "** Exponentiation  ➜  ^                       **\n"
         "** Absolute Value  ➜  abs()                   **\n"
         "**--------------------------------------------**\n"
         "**                   Samples                  **\n"
         "**--------------------------------------------**\n"
         "** (Input)  > \"(5+(10*(5+5)))\" -v             **\n"
         "** (Output) > 5 10 5 5 + * +                  **\n"
         "**          > Result: 105                     **\n"
         "**                                            **\n"
         "** (Input)  > \"10 + abs(15 %% 4 - 5*2)\" -v     **\n"
         "** (Output) > 10 15 4 %% 5 2 * - abs +         **\n"
         "**          > Result: 17                      **\n"
         "**--------------------------------------------**\n"
         "**                   Credits                  **\n"
         "**--------------------------------------------**\n"
         "**Created By: Michael Morikawa & Yazen Aljamal**\n"
         "**       Class: CPSC 223C - Spring 2022       **\n"
         "************************************************\n"
         "************************************************\n");
}