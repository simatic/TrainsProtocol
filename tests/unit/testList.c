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
  t_list *l2 = list_new();

  list_append(l, a);
  list_append(l, b);
  list_append(l, c);

  compare("removeFirst", list_removeFirst(l), a);
  compare("removeFirst", list_removeFirst(l), b);
  compare("removeFirst", list_removeFirst(l), c);

  // We now test list_extend and list_cleanList
  list_append(l, a);
  list_append(l2, b);
  list_append(l2, c);
  list_extend(l,l2);
  list_cleanList(l2);
  list_extend(l,l2);
  compare("removeFirst", list_removeFirst(l), a);
  compare("removeFirst", list_removeFirst(l), b);
  compare("removeFirst", list_removeFirst(l), c);

  // Code to check (with valgrind) that list_free frees everything
  list_free(l);
  list_free(l2);

  return EXIT_SUCCESS;
}

  
