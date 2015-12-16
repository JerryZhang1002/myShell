#include <stdio.h>
#include <cstdlib>
#include "stack.h"

dirStack * initStack() {
    dirStack * stack = (dirStack *) malloc(sizeof(*stack));
    stack->base = (char**) malloc((STACK_INIT_SIZE + 1)* sizeof(*(stack->base)));
    stack->top = stack->base;
    stack->stackLen = STACK_INIT_SIZE;
    return stack;
}

int push(dirStack * stack, char* newEle) {
    if (stack->top - stack->base == stack->stackLen) {        //stack is full, need to realloc space
        stack->base = (char**)realloc(stack->base, (stack->stackLen + STACK_INCREMENT + 1) * sizeof(*(stack->base)));
        if (!stack->base) {
            printf("Error: failed to allocate memory for dirStack!\n");
            return 0;                    //return 0 if added failed
        }
        stack->top = stack->base + stack->stackLen;
        stack->stackLen = stack->stackLen + STACK_INCREMENT;
    }
    stack->top = (stack->top) + 1;
    *(stack->top) = newEle;
    return 1;                            //return 1 if added successfully
}

char* pop(dirStack * stack) {           //the element being popped need to be freed by hand
    if (stack->top == stack->base) {
        return NULL;                    //empty stack, cannot pop
    }
    char* temp = *(stack->top);
    (stack->top)--;
    return temp;
}

void freeStack(dirStack * stack) {
    if (stack->top == stack->base) {
        free(stack->base);
        free(stack);
        printf("Stack is empty\n");
        return;
    }
    while (stack->top != stack->base) {
        free(*(stack->top));
        (stack->top)--;
    }
    free(stack->base);
    free(stack);
}

void printStack(dirStack * stack) {
    if (stack->top == stack->base) {
//        printf("Stack is empty\n");
        return;
    }
    char** temp = stack->top;
    while (temp != stack->base) {
        printf("%s\n",*temp);
        temp--;
    }
}
