#ifndef stack_h
#define stack_h

#define STACK_INIT_SIZE 3
#define STACK_INCREMENT 2

typedef struct dirStack {
    char** base;
    char** top;
    int stackLen;
}dirStack;

dirStack * initStack();
int push(dirStack * stack, char* newEle);
char* pop(dirStack * stack);
void freeStack(dirStack * stack);
void printStack(dirStack * stack);


#endif /* stack_h */
