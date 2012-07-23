/**
 Trains Protocol: Middleware for Uniform and Totally Ordered Broadcasts
 Copyright: Copyright (C) 2010-2012
 Contact: michel.simatic@telecom-sudparis.eu

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA

 Developer(s): Michel Simatic, Arthur Foltz, Damien Graux, Nicolas Hascoet, Nathan Reboud
 */

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

  
