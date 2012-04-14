#include <stdio.h>
#include <stdlib.h>

#include "list.h"

char *a = "A";
char *b = "B";
char *c = "C";

void compare(char *testType, void *s, char *target){
  printf("Testing %s...", testType);
  if ( (s == NULL) || (s != target)){
    printf("... KO (s is \"%s\" instead of \"%s\")\n", (char *)s, target);
    exit(EXIT_FAILURE);
  }
  printf("...OK\n");
}

int main() {
  t_list *l = list_new();

  list_append(l, a);
  list_append(l, b);
  list_append(l, c);

  compare("removeFirst", list_removeFirst(l), a);
  compare("removeFirst", list_removeFirst(l), b);
  compare("removeFirst", list_removeFirst(l), c);

  // Code to check (with valgrind) that list_free frees everything
  list_append(l, a);
  list_append(l, b);
  list_append(l, c);
  list_free(l);

  return EXIT_SUCCESS;
}

  
