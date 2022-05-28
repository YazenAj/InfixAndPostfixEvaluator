#include <stdio.h>

struct node;

typedef struct node Node;

typedef struct stack
{
    Node *entries;
    size_t size;
} Stack;

enum Errors
{
    valid = 0,
    STACK_UNDERFLOW
};

/**
 * @brief Create a Stack object.
 *
 * @return Stack* The new stack object.
 */
Stack *stack_create();

/**
 * @brief Releases the resources used by the given stack.
 */
void stack_free(Stack *stack);

/**
 * @brief Removes and returns the top item on the stack.
 *
 * @param stack The current stack.
 * @return long int The value on the top of the stack.
 */
long int stack_pop(Stack *stack, int *error);

/**
 * @brief Adds an item to the top of the stack.
 *
 * @param stack The current stack.
 * @param value The value to add to the top of the stack.
 */
void stack_push(Stack *stack, long int value);

/**
 * @brief Prints the values in the current stack.
 *
 * @param stack The current stack.
 */
void stack_print(Stack *stack);

/**
 * @brief Calculates the current size of the stack
 *
 * @param stack The current stack.
 * @return size_t The size of the stack.
 */
size_t stack_size(Stack *stack);

/**
 * @brief Checks if the stack is empty.
 *
 * @param stack The current stack.
 * @return int 1 if the stack is empty otherwise 0.
 */
int stack_empty(Stack *stack);