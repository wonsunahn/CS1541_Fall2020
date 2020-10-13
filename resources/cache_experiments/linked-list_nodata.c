#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define ACCESSES 1000000000

// Define a linked list node type with no data
typedef struct node {
  struct node* next;    // 8 bytes
  // no data now
} node_t;

// Keep a pointer to the beginning and end of the list
int items = 0;
node_t* head = NULL;
node_t* last = NULL;

void *create(void *unused) {
  // Repeatedly add items to the end of the list
  for(int i=0; i<items; i++) {
    // Allocate memory for the linked list node
    node_t* n = (node_t*)malloc(sizeof(node_t));
    
    // We're adding to the end, so the next pointer is NULL
    n->next = NULL;
    
    // Is the list empty? If so, the new node is the head and tail
    if(last == NULL) {
      head = n;
      last = n;
    } else {
      last->next = n;
      last = n;
    }
  }

}

void *run(void *unused) {
  // Now that we have a list, traverse the list over and over again until we've
  // visited `ACCESSES` nodes in our linked list.
  node_t* current = head;
  for(int i=0; i<ACCESSES; i++) {
    if(current == NULL) current = head;
    else current = current->next;
  }

  return NULL;
}

int main(int argc, char** argv) {
  // Check to make sure we received a command line option
  if(argc < 2) {
    fprintf(stderr, "Usage: %s <list size>\n", argv[0]);
    return 1;
  }

  // How many items should we have in the list?
  items = atoi(argv[1]);

  pthread_t tCreate, tRun;
  pthread_create(&tCreate, NULL, create, NULL);
  pthread_join(tCreate, NULL);

  pthread_create(&tRun, NULL, run, NULL);
  pthread_join(tRun, NULL);

  return 0;
}
