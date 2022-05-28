#include <stdlib.h>
#include "stack.h"

struct node {
    long int value;
    struct node *next;
};

Node *node_create(long int value)
{
    Node *new_node = (Node *)malloc(sizeof *new_node);
    new_node->value = value;
    new_node->next = NULL;
    return new_node;
}

Stack *stack_create()
{
    Stack *temp = (Stack *)malloc(sizeof *temp);
    temp->entries = NULL;
    temp->size = 0;
    return temp;
}

void stack_free(Stack *stack)
{
  if (!stack)
    return;
  
  Node *node = stack->entries;
  while (node)
  {
    Node *next = node->next;
    free(node);
    node = next;
  }
  free(stack);
}

long int stack_pop(Stack *stack, int *errno)
{
    if (stack->entries == NULL)
    {
        *errno = STACK_UNDERFLOW;
        return 0;
    }

    Node *top = stack->entries;
    long int val = top->value;
    stack->entries = stack->entries->next;
    free(top);
    stack->size--;
    return val;
}

void stack_push(Stack *stack, long int value)
{
    Node *temp = node_create(value);

    if (stack->entries == NULL)
    {
        stack->entries = temp;
    }
    else
    {
        temp->next = stack->entries;
        stack->entries = temp;
    }
  
    stack->size++;
}

void stack_print(Stack *stack)
{
    if (stack)
    {
      Node *top = stack->entries;
      while(top)
      {
        printf("|%ld\n", top->value);
        if (!top->next)
        {
            printf("----\n");
        }
        top = top->next;
      }
    }
    else
    {
      printf("|Empty\n----");
    }
}

size_t stack_size(Stack *stack)
{
  int count = 0;
  Node* node = stack->entries;
  while (node)
  {
    node = node->next;
    ++count;
  }
  return count;
}

int stack_empty(Stack *stack)
{
    return stack == NULL || stack->size == 0;
}