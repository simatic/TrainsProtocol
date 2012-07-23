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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#include "management_addr.h"

ADDR* global_addr_array;

// Max length of a line read in LOCALISATION file
#define MAX_LEN_LINE_IN_FILE (MAX_LEN_RANK + 1 + MAX_LEN_IP + 1 + MAX_LEN_CHAN)

//create an array of addresses
ADDR* init_addr_list(int length){
  ADDR* result=calloc(length+1,sizeof(ADDR));//We calloc one more element, so that the last element of the array is an empty element
  assert(result != NULL);
  return(result);
}

//free an array of addresses
void free_addr_list(ADDR* tab){
  free(tab);
}

//read the file of addresses to fulfill our array
//it takes to arguments : the place where is the file and the maximal number of addresses it could have.
ADDR* addr_generator(char* locate, int length){
  FILE * addr_file;
  ADDR * array=init_addr_list(length);
  char line[MAX_LEN_LINE_IN_FILE];
  char * addr_full=NULL;
  char * ip_only=NULL;
  char * rank_str=NULL;
  int rank;
  bool already_exist[16];

  int i=0;
  for (i = 0; i < 16; i++) {
    already_exist[i] = false;
  }

  addr_file = fopen (locate , "r");
  if (addr_file == NULL )
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "Error opening file");
  else {
    i=0;
      while (fgets(line, MAX_LEN_LINE_IN_FILE, addr_file) != NULL ) {
        i++;
        if ((line[0] != '#') && (line[0] != '\n')) {
          rank_str = strtok(line, ":");
          rank = atoi(rank_str);
          ip_only = strtok(NULL, ":");
          addr_full = strtok(NULL, ":\n");
          if (rank >= 16 ||
              rank < 0 ||
              ip_only == NULL ||
              addr_full == NULL ||
              already_exist[rank]) {
            fprintf(stderr,
                "BAD USE OF addr_file AT LINE %d\n\n"
                "Each line should be NB:HOSTNAME:PORT\n\t"
                "NB being a number between 0 and 15\n\t"
                "HOSTNAME being the... hostname ! (max length 63 char)\n\t"
                "PORT being the port on which the process works (max length 63 char)\n\n"
                "Comments line allowed (begin with #), empty lines allowed\n", i);
            exit(-1) ;
          }
          already_exist[rank] = true;
          strcpy(array[rank].ip, ip_only);
          strcpy(array[rank].chan, addr_full);
        }
      }
  }
  fclose(addr_file);

  return(array);
}

//add a tcomm to a place in an ADDR*
void add_tcomm(t_comm * tcomm, int i, ADDR * array, bool isPred){
  if (array[i].tcomm[0] == NULL) {
    array[i].tcomm[0]=tcomm;
    array[i].isPred[0]=isPred;
  } else {
    array[i].tcomm[1]=tcomm;
    array[i].isPred[1]=isPred;
  }
}

//Tries to return a non-NULL t_comm at place i in an ADDR*
t_comm *get_tcomm(int i, bool isPred, ADDR * array){
  if ((array[i].tcomm[0] != NULL) && (array[i].isPred[0] == isPred)) {
    return array[i].tcomm[0];
  } else if (array[i].isPred[1] == isPred) {
    return array[i].tcomm[1];
  } else {
    return NULL;
  }
}

//remove a tcomm from a place in an ADDR*
void remove_tcomm(t_comm * tcomm, int i, ADDR * array){
  if (array[i].tcomm[0] == tcomm){
    array[i].tcomm[0] = NULL;
  } else if (array[i].tcomm[1] == tcomm){
    array[i].tcomm[1] = NULL;
  }
}

//search a t_comm in an array
void search_tcomm(t_comm * tcomm, ADDR * array, int *prank, bool *pisPred){
  int i=0;

  *prank=-1;
  for(i=0;i<NP;i++){
    if(array[i].tcomm[0] == tcomm){
      *prank=i;
      *pisPred=array[i].isPred[0];
      return;
    } else if(array[i].tcomm[1] == tcomm){
      *prank=i;
      *pisPred=array[i].isPred[1];
      return;
    }
  }
}

//give the place in the array of a given address
//return -1 if it's unfound
int addr_id(char * ip, char * chan, ADDR * array){
  int i=0;

  while((array[i].ip[0] != '\0')&&(strcmp(array[i].ip,ip)!=0 || strcmp(array[i].chan,chan)!=0)){
    i++;
  }

  if(array[i].ip[0] != '\0'){
    return(i);
  }
  else{
    return(-1);
  }
}
