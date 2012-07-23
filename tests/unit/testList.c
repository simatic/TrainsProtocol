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
  t_list *l = newList();
  t_list *l2 = newList();

  listAppend(l, a);
  listAppend(l, b);
  listAppend(l, c);

  compare("removeFirst", listRemoveFirst(l), a);
  compare("removeFirst", listRemoveFirst(l), b);
  compare("removeFirst", listRemoveFirst(l), c);

  // We now test listExtend and cleanList
  listAppend(l, a);
  listAppend(l2, b);
  listAppend(l2, c);
  listExtend(l,l2);
  cleanList(l2);
  listExtend(l,l2);
  compare("removeFirst", listRemoveFirst(l), a);
  compare("removeFirst", listRemoveFirst(l), b);
  compare("removeFirst", listRemoveFirst(l), c);

  // Code to check (with valgrind) that freeList frees everything
  freeList(l);
  freeList(l2);

  return EXIT_SUCCESS;
}

  
