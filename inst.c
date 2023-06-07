#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h> 
#include <sys/stat.h>


#ifndef STRUCT_PRESENT
    #include "structures.c" //Used for Linked List, Array List, and Function Info data structures
#endif 


Node *functionLog;
void *firstFunc;

sem_t semaphore;
int semaphoreInitiated = 0;




__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void *this_fn, void *call_site){

    if(semaphoreInitiated == 0){
        if (sem_init(&semaphore, 1, 1) == -1) {
            perror("Error initializing semaphore");
        }
        semaphoreInitiated = 1;
    }
    


    // Initialize FunctionInfo struct
    FunctionInfo info = {
        .currentFunc = this_fn,
        .callingFunc = call_site,
        .profiledFrame = __builtin_frame_address(1),
        .stack = __builtin_frame_address(1) + 2 * sizeof(void *),
        .stackEnd = __builtin_frame_address(0) + 4 * sizeof(void *),
        .stackContents = {0}

    };

    clock_gettime(CLOCK_MONOTONIC, &info.clock);

    // Use memcpy to copy the contents of the stack to the stackContents list
    memcpy(info.stackContents, info.stack, sizeof(unsigned long) * DEFAULT_SIZE);

    // Add the FunctionInfo struct to a linked list or other data structure
    // for further processing and analysis...

    if(functionLog == NULL){
        functionLog = newNode(info);
        firstFunc = this_fn;
    } else {
        addNode(functionLog, newNode(info));
        functionLog = functionLog->next;
    }

}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *this_fn, void *call_site){

    // Initialize FunctionInfo struct
    FunctionInfo info = {
        .process = getpid(),
        .parent = getppid(),
        .currentFunc = this_fn,
        .callingFunc = call_site,
        .profiledFrame = __builtin_frame_address(1),
        .stack = __builtin_frame_address(1) + 2 * sizeof(void *),
        .stackEnd = __builtin_frame_address(0) + 4 * sizeof(void *),
        .stackContents = {0}
    };

    clock_gettime(CLOCK_MONOTONIC, &info.clock);

    // Use memcpy to copy the contents of the stack to the stackContents list
    memcpy(info.stackContents, info.stack, sizeof(unsigned long) * DEFAULT_SIZE);


    // Add the FunctionInfo struct to a linked list for further processing and analysis...

    if(functionLog == NULL){
        functionLog = newNode(info);
    } else {
        addNode(functionLog, newNode(info));
        functionLog = functionLog->next;
    }

    if(this_fn == firstFunc){ //This will run once the very fist function ends (main)

        // Traverse to the first node in the linked list
        while(functionLog->last != NULL){
            functionLog = functionLog->last;
        }

        if(sem_wait(&semaphore) == -1){
            perror("Error waiting for semaphore");
        }
    
        // Open a binary file for writing
        FILE *binaryFile = fopen("stack.bin", "a+b");
        if (binaryFile == NULL) {
            perror("Error opening file");
        }

        // Write the function log linked list to the binary file
        Node *current = functionLog;
        while (current != NULL) {
            // Write the FunctionInfo data to the binary file
            if (fwrite(&current->data, sizeof(FunctionInfo), 1, binaryFile) != 1) {
                perror("Error writing to file");
            }
            // Move to the next node in the linked list
            current = current->next;
        }

        // Close the binary file
        fclose(binaryFile);
    
        if(sem_post(&semaphore) == -1){
            perror("Error waiting for semaphore");
        }
        
        
    }

}
