#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <error.h>

#include "bqueue.h"
#include "list.h"

t_bqueue *q;

char *a = "A";
char *b = "B";
char *c = "C";

void *functionThread1(void *null) {
  bqueue_enqueue(q, a);
  bqueue_enqueue(q, b);
  bqueue_enqueue(q, c);
  return NULL;
}

void compare(void *s, char *target){
  if ( (s == NULL) || (s != target)){
    printf("... KO (s is \"%s\" instead of \"%s\")\n", (char *)s, target);
    exit(EXIT_FAILURE);
  }
}

int main() {
  int rc;
  pthread_t thread;
  t_list *l;

  q = bqueue_new();

  printf("testing bqueue...");
  fflush(stdout); // Pour avoir l'affichage ci-dessus, même si on se bloque pendant le test

  bqueue_enqueue(q, a);
  bqueue_enqueue(q, b);
  bqueue_enqueue(q, c);

  compare(bqueue_dequeue(q), a);
  compare(bqueue_dequeue(q), b);
  compare(bqueue_dequeue(q), c);

  rc = pthread_create(&thread, NULL, &functionThread1, NULL);
  if (rc < 0)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");

  compare(bqueue_dequeue(q), a);
  compare(bqueue_dequeue(q), b);
  compare(bqueue_dequeue(q), c);

  l = list_new();
  bqueue_extend(q,l);
  list_append(l,a);
  list_append(l,b);
  list_append(l,c);
  bqueue_extend(q,l);
  list_cleanList(l);
  list_append(l,a);
  list_append(l,b);
  list_append(l,c);
  bqueue_extend(q,l);

  compare(bqueue_dequeue(q), a);
  compare(bqueue_dequeue(q), b);
  compare(bqueue_dequeue(q), c);
  compare(bqueue_dequeue(q), a);
  compare(bqueue_dequeue(q), b);
  compare(bqueue_dequeue(q), c);
  
  // free memory 
  rc = pthread_join(thread, NULL);
  if(rc)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_join");

  bqueue_free(q);

  printf("OK\n");

  return EXIT_SUCCESS;
}

  
