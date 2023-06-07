#include <stdio.h>
#include <time.h>

#ifndef STRUCT_PRESENT
    #include "structures.c"
#endif


// Visualize the stack frame and call information for each pair of FunctionInfo structures
__attribute__((no_instrument_function))
void visualizeStackFrames(FunctionInfo first, FunctionInfo second) {
    // Print the function, caller, and frame pointer information
    printf("start process: %d, start parent process: %d\n",
        first.process,
        first.parent);

    printf("end process: %d, end parent process: %d\n",
        second.process,
        second.parent);

    printf("function: %p, caller: %p, frame pointer: %p\n",
        first.currentFunc,
        first.callingFunc,
        first.profiledFrame);

    // Print the stack frame information
    printf("stack frame: %p-%p, time: (%ld.%.9ld - %ld.%.9ld)\n",
        first.stackEnd,
        first.profiledFrame,
        first.clock.tv_sec, first.clock.tv_nsec,
        second.clock.tv_sec, second.clock.tv_nsec);

    // Print the address range and initial/final values for each element in the stack frame

    printf("address range                  initial    final\n");

    int j = 0;

    for (void *i = first.stackEnd; j < DEFAULT_SIZE; i++, j++) {
        printf("%p-%p: %08x | %08x\n",
            i, i - sizeof(void *),
            first.stackContents[i - first.stackEnd],
            second.stackContents[i - second.stackEnd]);
    }
    printf("\n");
}

// Function to compare two FunctionInfo structs by their start time
int compareByTime(const void *a, const void *b) {
    FunctionInfo *ia = (FunctionInfo *)a;
    FunctionInfo *ib = (FunctionInfo *)b;
    if (ia->clock.tv_sec == ib->clock.tv_sec) {
        return (ia->clock.tv_nsec - ib->clock.tv_nsec);
    }
    else {
        return (ia->clock.tv_sec - ib->clock.tv_sec);
    }
}

int compareByTime_Inverse(const void *a, const void *b) {
    FunctionInfo *ia = (FunctionInfo *)a;
    FunctionInfo *ib = (FunctionInfo *)b;
    if (ia->clock.tv_sec == ib->clock.tv_sec) {
        return (ib->clock.tv_nsec - ia->clock.tv_nsec);
    }
    else {
        return (ib->clock.tv_sec - ia->clock.tv_sec);
    }
}

int compareByTime_GroupedByPID(const void *a, const void *b) {
    FunctionInfo *ia = (FunctionInfo *)a;
    FunctionInfo *ib = (FunctionInfo *)b;
    if (ia->clock.tv_sec == ib->clock.tv_sec) {
        if (ia->process == ib->process) {
            return (ia->clock.tv_nsec - ib->clock.tv_nsec);
        }
        else {
            return (ia->process - ib->process);
        }
    }
    else {
        return (ia->clock.tv_sec - ib->clock.tv_sec);
    }
}




int main(int argc, char* argv[]){

    if(argc < 2){
        perror("Incorrect arguments. Ex: stackviz.c stack.bin");
        return -1;
    }

    // Read the function log linked list from a binary file
    // and store it in a linked list of Node structures

    // Open the binary file for reading
    FILE *binaryFile = fopen(argv[1], "rb");
    if (binaryFile == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Read the function log linked list from the binary file
    Node *functionLog = NULL;
    Node *current = functionLog;
    FunctionInfo info = (FunctionInfo){0}; //Init as all 0s

    // Read the FunctionInfo data from the binary file until EOF is reached
    while (fread(&info, sizeof(FunctionInfo), 1, binaryFile) == 1) { 

        // Add the FunctionInfo struct to a new node in the linked list

        if(current == NULL){ //Beginning of list case
            current = newNode(info);
        } else {
            addNode(current, newNode(info));
            current = current->next;
        }

        //Empty info variable for next iteration
        info = (FunctionInfo){0};

    }

    if(argc < 3){
        
        // Traverse to the first node in the linked list

        while(current->last != NULL){
            current = current->last;
        }

        // Visualize the stack frame and call information for each pair of FunctionInfo structures
        int count = 1;

        while(current->next != NULL){
            printf("(%d)\n", count); 
            count++;
            visualizeStackFrames(current->data, current->next->data);
            current = current->next;
        }

    } else {

        // Create an empty array to store linked list contents
        FunctionInfo *functionArray = NULL;
        int arraySize = 0;

        // Traverse linked list and store data in array
        while(current->last != NULL){
            // Increase array size by one and reallocate memory
            arraySize++;
            functionArray = realloc(functionArray, sizeof(FunctionInfo) * arraySize);

            // Store current data in array and move to next element in linked list
            functionArray[arraySize - 1] = current->data;
            current = current->last;
        }

        switch (argv[2][1]) {
            case 's':
                qsort(functionArray, arraySize, sizeof(FunctionInfo), compareByTime);
                break;
        
            case 'f':
                qsort(functionArray, arraySize, sizeof(FunctionInfo), compareByTime_Inverse);
                break;

            case 'p':
                qsort(functionArray, arraySize, sizeof(FunctionInfo), compareByTime_GroupedByPID);
                break;

            default:
                break;
        }

        for(int i = 0; i < arraySize - 1; i++){
            printf("(%d)\n", i + 1);
            visualizeStackFrames(functionArray[i], functionArray[i+1]);
        }

        // Free memory when done
        free(functionArray);
       
    }


}

