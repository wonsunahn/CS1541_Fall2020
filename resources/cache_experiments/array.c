#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define ACCESSES 1000000000

// Define a linked list node type with no data
typedef struct node {
  struct node* next;    // 8 bytes
  int data[30];         // 120 bytes
} node_t;


// Keep a pointer to the beginning and end of the list
int items = 0;
node_t *array = NULL;

void *create(void *unused) {
  array = (node_t *) malloc(sizeof(node_t) * items);
  for(int i=0; i<items; i++) {
    // Emulate the same mallocs as in linked list
    for(int j=0; j<10; j++) {
      (node_t*)malloc(sizeof(node_t));
    }
  }
}

void *run(void *unused) {
  // Now that we have an array, traverse the array over and over again until we've
  // visited `ACCESSES` elements in our array.
  node_t* current = array;
  for(int i=0; i<ACCESSES; i++) {
    if(current == array + items) current = array;
    else ++current;
  }

  return NULL;
}

int main(int argc, char** argv) {
  // Check to make sure we received a command line option
  if(argc < 2) {
    fprintf(stderr, "Usage: %s <array size>\n", argv[0]);
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
