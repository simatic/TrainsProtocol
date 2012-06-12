#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comm.h"
#include "management_addr.h"

#define NB_LINES_IN_FILE 16
#define FAKE_COMM1 ((t_comm*)0xFFFFFFFF)
#define FAKE_COMM2 ((t_comm*)0x8FFFFFFF)
#define SEARCHED_INDEX 8

void compare(char *testType, bool result){
  printf("Testing %s...", testType);
  if (!result){
    printf("... KO\n");
    exit(EXIT_FAILURE);
  }
  printf("...OK\n");
}

int main(){
  int i;
  char ip[MAX_LEN_IP];
  char chan[MAX_LEN_CHAN];

  global_addr_array = addr_generator(LOCALISATION, NB_LINES_IN_FILE);

  for (i=0; i<NB_LINES_IN_FILE; i++){
    memset(ip,65+i,MAX_LEN_IP);
    ip[MAX_LEN_IP-1] = '\0';
    memset(chan,97+i,MAX_LEN_CHAN);
    chan[MAX_LEN_CHAN-1] = '\0';
    compare("global_addr_array contents", ((strcmp(ip,global_addr_array[i].ip)==0) && (strcmp(chan,global_addr_array[i].chan)==0) && (global_addr_array[i].tcomm[0]==NULL) && (global_addr_array[i].tcomm[1]==NULL)));
  }
  compare("global_addr_array contents", ((global_addr_array[i].ip[0]=='\0') && (global_addr_array[i].chan[0]=='\0') && (global_addr_array[i].tcomm[0]==NULL) && (global_addr_array[i].tcomm[1]==NULL)));

  add_tcomm(FAKE_COMM1, 0, global_addr_array);
  add_tcomm(FAKE_COMM2, 0, global_addr_array);
  compare("add_tcomm", global_addr_array[0].tcomm[0]==FAKE_COMM1);
  compare("add_tcomm", global_addr_array[0].tcomm[1]==FAKE_COMM2);

  compare("get_tcomm", get_tcomm(0,global_addr_array)==FAKE_COMM1);
  remove_tcomm(FAKE_COMM1, 0, global_addr_array);
  compare("get_tcomm", get_tcomm(0,global_addr_array)==FAKE_COMM2);
  add_tcomm(FAKE_COMM1, 0, global_addr_array);
  compare("get_tcomm", get_tcomm(0,global_addr_array)==FAKE_COMM1);

  memset(ip,65+SEARCHED_INDEX,MAX_LEN_IP);
  ip[MAX_LEN_IP-1] = '\0';
  memset(chan,97+SEARCHED_INDEX,MAX_LEN_CHAN);
  chan[MAX_LEN_CHAN-1] = '\0';
  compare("addr_id", addr_id(ip, chan, global_addr_array) == SEARCHED_INDEX);
  chan[0] = '\0';
  compare("addr_id", addr_id(ip, chan, global_addr_array) == -1);

  remove_tcomm(FAKE_COMM2, 0, global_addr_array);
  compare("remove_tcomm", global_addr_array[0].tcomm[0]==FAKE_COMM1);
  compare("remove_tcomm", global_addr_array[0].tcomm[1]==NULL);
  remove_tcomm(FAKE_COMM1, 0, global_addr_array);
  compare("remove_tcomm", global_addr_array[0].tcomm[0]==NULL);
  compare("remove_tcomm", global_addr_array[0].tcomm[1]==NULL);


  free_addr_list(global_addr_array);

  return EXIT_SUCCESS;
}
