#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "address.h"

address my_address = 0x0001;
address null_address = 0x0000;
char s[MAX_LEN_ADDRESS_AS_STR];

void compare(char *testType, int ad, int target){
  printf("Testing %s...", testType);
  if (ad != target){
    printf("... KO (ad is \"%d\" instead of \"%d\")\n", ad, target);
    exit(EXIT_FAILURE);
  }
  printf("...OK\n");
}

int main() {
  address_set arrivedSet;
  address_set goneSet;

  compare("addr_isnull", addr_isnull(null_address), true);
  compare("addr_isnull", addr_isnull(my_address), false);

  compare("addr_cmp", addr_cmp(null_address,my_address), true);
  compare("addr_cmp", addr_cmp(my_address,null_address), false);

  compare("addr_isequal", addr_isequal(null_address,null_address), true);
  compare("addr_isequal", addr_isequal(my_address,null_address), false);

  compare("addr_ismine", addr_ismine(my_address), true);
  compare("addr_ismine", addr_ismine(null_address), false);

  printf("Testing %s...", "addr_2_str");
  addr_2_str(s,my_address);
  if (strcmp(s, "#00") != 0){
    printf("... KO (s is \"%s\" instead of \"%s\")\n", s, "#00");
    exit(EXIT_FAILURE);
  }
  printf("...OK\n");

  compare("addr_2_rank", addr_2_rank(my_address), 0);
  compare("addr_2_rank", addr_2_rank(0x8000), 15);
  compare("addr_2_rank", addr_2_rank(null_address), -1);

  compare("rank_2_addr", rank_2_addr(0), my_address);
  compare("rank_2_addr", rank_2_addr(15), 0x8000);
  compare("rank_2_addr", rank_2_addr(16), null_address);

  compare("addr_ismember", addr_ismember(my_address, 0xFFFF), true);
  compare("addr_ismember", addr_ismember(my_address, 0xFFFE), false);

  arrivedSet = 0xFFFE;
  addr_appendArrived(&arrivedSet, my_address);
  compare("addr_appendArrived", arrivedSet, 0xFFFF);
  arrivedSet = 0x0000;
  addr_appendArrived(&arrivedSet, my_address);
  compare("addr_appendArrived", arrivedSet, 0x0001);

  arrivedSet = 0x0000;
  goneSet = 0xFFFE;
  addr_appendGone(&arrivedSet, &goneSet, my_address);
  printf("Testing %s...", "addr_appendGone");
  if ((arrivedSet != 0x0000) || (goneSet != 0xFFFF)){
    printf("... KO (arrivedSet is \"%d\" instead of \"%d\" OR goneSet is \"%d\" instead of \"%d\")\n", arrivedSet, 0x0000, goneSet, 0xFFFF);
    exit(EXIT_FAILURE);
  }
  printf("...OK\n");
  arrivedSet = 0xFFFF;
  goneSet = 0x0000;
  addr_appendGone(&arrivedSet, &goneSet, my_address);
  printf("Testing %s...", "addr_appendGone");
  if ((arrivedSet != 0xFFFE) || (goneSet != 0x0000)){
    printf("... KO (arrivedSet is \"%d\" instead of \"%d\" OR goneSet is \"%d\" instead of \"%d\")\n", arrivedSet, 0xFFFF, goneSet, 0x0000);
    exit(EXIT_FAILURE);
  }
  printf("...OK\n");

  compare("addr_updateCircuit", addr_updateCircuit(0xFFFF, my_address, 0xFF00, 0x00FF), 0xFF00);

  return EXIT_SUCCESS;
}

  
