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
#include <unistd.h>

#include "management_addr.h"

ADDR* globalAddrArray;

// Max length of a line read in LOCALISATION file
#define MAX_LEN_LINE_IN_FILE (MAX_LEN_RANK + 1 + MAX_LEN_IP + 1 + MAX_LEN_CHAN)

//create an array of addresses
ADDR* initAddrList(int length){
  ADDR* result=calloc(length+1,sizeof(ADDR));//We calloc one more element, so that the last element of the array is an empty element
  assert(result != NULL);
  return(result);
}

//free an array of addresses
void freeAddrList(ADDR* tab){
  free(tab);
}

//read the file of addresses to fulfill our array
//it takes to arguments : the place where is the file and the maximal number of addresses it could have.
ADDR* addrGenerator(char* locate, int length){
  FILE * addrFile;
  ADDR * array=initAddrList(length);
  char * line = malloc(MAX_LEN_LINE_IN_FILE * sizeof(char));
  char * addr_full = NULL;
  char * ip_only = NULL;
  char * rank_str = NULL;
  int rank;
  bool already_exist[length];
  int currentLine;
  int i=0;
  for (i = 0; i < length; i++) {
    already_exist[i] = false;
  }

  addrFile = fopen (locate , "r");
  if (addrFile == NULL )
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "Error opening file");
  else {
    currentLine=0;
      while (fgets(line, MAX_LEN_LINE_IN_FILE, addrFile) != NULL ) {
      currentLine++;

      //Delete the spaces at the beginning of the line
      int spaces = 0;
      while (line[spaces] == ' ') {
        spaces++;
      }
      line = &line[spaces];

      if ((line[0] != '#') && (line[0] != '\n')) {
        rank_str = strtok(line, ":");
        rank = atoi(rank_str);
        ip_only = strtok(NULL, ":");
        addr_full = strtok(NULL, ":\n");
        // Error manager
        if (rank < 0 || rank >= 16 || ip_only == NULL || addr_full == NULL
            || already_exist[rank]) {

          char * errorType = malloc(64*sizeof(char));

          if (rank < 0 || rank >= 16) {
            strcpy(errorType, "RANK error : should be between 0 and 15");
          } else if (ip_only == NULL || addr_full == NULL ) {
            strcpy(errorType, "RANK:HOSTNAME:PORT Semantic error");
          } else if (already_exist[rank]) {
            strcpy(errorType, "RANK already exists in a previous line");
          }

          error_at_line(EXIT_FAILURE, 0, __FILE__, __LINE__,
              "\n%s:%d: %s\n\n"
                  "Each line should be RANK:HOSTNAME:PORT\n\t"
                  "RANK being a number between 0 and 15\n\t"
                  "HOSTNAME being the... hostname ! (max length 63 char)\n\t"
                  "PORT being the port on which the process works (max length 63 char)\n\n"
                  "Comments line allowed (begin with #), empty lines allowed\n",
              locate, currentLine, errorType);

          free(errorType);
        }

        already_exist[rank] = true;

        // if localhost is detected in addr_file, we replace it with the actual hostname
        if (!strcmp(ip_only, "localhost")) {
          char ip_only_for_hostname[64];
          i = gethostname(ip_only_for_hostname, 64);
          if (i < 0) {
            error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
                "Error getting hostname");
          }
          ip_only = ip_only_for_hostname;
        }

        for (i = 0; i < length; i++) {
          if (!strcmp(array[i].ip, ip_only)) {
            if (!strcmp(array[i].chan, addr_full)) {
              error_at_line(EXIT_FAILURE, 0, __FILE__, __LINE__,
                  "\n%s:%d: This participant already exists in a previous line\n"
                      "Each hostname:port pair should be unique\n",
                  locate, currentLine);
            }
          }
        }

        strcpy(array[rank].ip, ip_only);
        strcpy(array[rank].chan, addr_full);
      }
    }
  }
  fclose(addrFile);

  for (i = 0; i < NP; i++){
    if (!already_exist[i]){
      strcpy(array[i].ip, "localhost");
      strcpy(array[i].chan,"3000");
    }
  }

  free(line);
  return(array);
}

//add a tcomm to a place in an ADDR*
void addTComm(t_comm * tcomm, int i, ADDR * array, bool isPred){
  if (array[i].tcomm[0] == NULL) {
    array[i].tcomm[0]=tcomm;
    array[i].isPred[0]=isPred;
  } else {
    array[i].tcomm[1]=tcomm;
    array[i].isPred[1]=isPred;
  }
}

//Tries to return a non-NULL t_comm at place i in an ADDR*
t_comm *getTComm(int i, bool isPred, ADDR * array){
  if ((array[i].tcomm[0] != NULL) && (array[i].isPred[0] == isPred)) {
    return array[i].tcomm[0];
  } else if (array[i].isPred[1] == isPred) {
    return array[i].tcomm[1];
  } else {
    return NULL;
  }
}

//remove a tcomm from a place in an ADDR*
void removeTComm(t_comm * tcomm, int i, ADDR * array){
  if (array[i].tcomm[0] == tcomm){
    array[i].tcomm[0] = NULL;
  } else if (array[i].tcomm[1] == tcomm){
    array[i].tcomm[1] = NULL;
  }
}

//search a t_comm in an array
void searchTComm(t_comm * tcomm, ADDR * array, int *prank, bool *pisPred){
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
int addrID(char * ip, char * chan, ADDR * array){
  int i=0;

  while ((array[i].ip[0] != '\0')
      && (strcmp(array[i].ip, ip) != 0 || strcmp(array[i].chan, chan) != 0)) {
    i++;
  }

  if(array[i].ip[0] != '\0'){
    return(i);
  }
  else{
    return(-1);
  }
}
