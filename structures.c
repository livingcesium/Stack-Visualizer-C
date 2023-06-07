#include "structures.h"

// Linked-List pseudo-methods

__attribute__((no_instrument_function))
Node* newNode(FunctionInfo data){

	Node *output = malloc( sizeof(Node) );

	if (output == NULL){ 
        perror("Memory Error while storing stack contents");
        return NULL;
    }
	output->data = data;

    output->last = NULL;

	output->next = NULL;

	return output;

}

__attribute__((no_instrument_function))
void addNode(Node *current, Node *newNode) {

    current->next = newNode;

    newNode->last = current;

}

__attribute__((no_instrument_function))
void reverseList(Node** head) {
  Node* current = *head;
  Node* previous = NULL;
  Node* next = NULL;

  while (current != NULL) {
    next = current->next;
    current->next = previous;
    previous = current;
    current = next;
  }

  *head = previous;
}

// End of Linked-List pseudo-methods