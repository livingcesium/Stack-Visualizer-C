#include <stdlib.h>
#include <time.h>

#define STRUCT_PRESENT 1 //Used with ifdef to ensure no redundant includes of this file are done
#define DEFAULT_SIZE 8 //Default number of addresses to store

//This file is used to store all of the data structures I used for this project
//Linked Lists (Node) are implemented here
//A collection of pointers and a fixed array is used to gather function info.

typedef struct FunctionInfo{

    pid_t process; pid_t parent;

    void *currentFunc; void *profiledFrame;
    void *callingFunc; void *stack; void *stackEnd;
    struct timespec clock; 
    long stackContents[8];

} FunctionInfo;



typedef struct Node{

    FunctionInfo data; 
    struct Node *next;
    struct Node *last;

} Node;



Node* newNode(FunctionInfo data);

void addNode(Node *current, Node *newNode);