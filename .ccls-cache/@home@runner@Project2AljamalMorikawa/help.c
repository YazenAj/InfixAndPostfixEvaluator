#include <stdio.h>
#include "help.h"

void help_menu()
{
  char choice[64];
  print_header();

  while (1)
  {
    fgets(choice, 64, stdin);
    switch (choice[0])
    {
    case '1':
      print_commands();
      break;
    case '2':
      print_operators();
      break;
    case '3':
      print_sample();
      break;
    case '4':
      print_credits();
      break;
    case 'q':
    case '5':
      printf("Good Bye!\n");
      return;
    default:
      printf("That is not a valid choice.\n");
      break;
    }
  }
}

void print_header()
{
  printf("************************************************\n"
         "************************************************\n"
         "**          Infix and Postfix Parser          **\n"
         "**--------------------------------------------**\n"
         "**                  Main Menu                 **\n"
         "**--------------------------------------------**\n"
         "** 1. Commands                                **\n"
         "** 2. Operators                               **\n"
         "** 3. Sample                                  **\n"
         "** 4. Credits                                 **\n"
         "** 5. Exit                                    **\n"
         "************************************************\n"
         "************************************************\n");
}

void print_commands()
{
  printf("**--------------------------------------------**\n"
         "**                  Commands                  **\n"
         "**--------------------------------------------**\n"
         "** -h......................Displays Help Menu **\n"
         "** -d....................Toggles Debug Output **\n"
         "** -v..................Display Postfix Result **\n"
         "** -r....................Set input to PostFix **\n"
         "**--------------------------------------------**\n\n");
}

void print_operators()
{
  printf("**--------------------------------------------**\n"
         "**                  Operators                 **\n"
         "**--------------------------------------------**\n"
         "** Addition        ➜  +                       **\n"
         "** Subtraction     ➜  -                       **\n"
         "** Division        ➜  /                       **\n"
         "** Multiplication  ➜  *                       **\n"
         "** Modulo          ➜  %%                       **\n"
         "** Exponentiation  ➜  ^                       **\n"
         "**--------------------------------------------**\n\n");
}

void print_sample()
{
  printf("**--------------------------------------------**\n"
         "**                   Sample                   **\n"
         "**--------------------------------------------**\n"
         "** (Input)  > \"(5+(10*(5+5)))\" -v             **\n"
         "** (Output) > 5 10 5 5 + * +                  **\n"
         "**          > 105                             **\n"
         "**--------------------------------------------**\n\n");
}

void print_credits()
{
  printf("**--------------------------------------------**\n"
         "**                   Credits                  **\n"
         "**--------------------------------------------**\n"
         "**Created By: Michael Morikawa & Yazen Aljamal**\n"
         "**       Class: CPSC 223C - Spring 2022       **\n"
         "**--------------------------------------------**\n");
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
         "**--------------------------------------------**\n"
         "**                  Operators                 **\n"
         "**--------------------------------------------**\n"
         "** Addition        ➜  +                       **\n"
         "** Subtraction     ➜  -                       **\n"
         "** Division        ➜  /                       **\n"
         "** Multiplication  ➜  *                       **\n"
         "** Modulo          ➜  %%                       **\n"
         "** Exponentiation  ➜  ^                       **\n"
         "**--------------------------------------------**\n"
         "**                   Sample                   **\n"
         "**--------------------------------------------**\n"
         "** (Input)  > \"(5+(10*(5+5)))\" -v             **\n"
         "** (Output) > 5 10 5 5 + * +                  **\n"
         "**          > 105                             **\n"
         "**--------------------------------------------**\n"
         "**                   Credits                  **\n"
         "**--------------------------------------------**\n"
         "**Created By: Michael Morikawa & Yazen Aljamal**\n"
         "**       Class: CPSC 223C - Spring 2022       **\n"
         "************************************************\n"
         "************************************************\n");
}