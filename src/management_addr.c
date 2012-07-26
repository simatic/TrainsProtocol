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
#define MAX_LEN_LINE_IN_FILE 1024

//give the length of a char *, or 0 if the char * == NULL
//this function has been created because strlen was not sufficient for our purpose
int stringLength(char * stringToEvaluate){
  int i = 0;

  if (stringToEvaluate == NULL )
    return 0;
  while (stringToEvaluate[i] != '\0') {
    i++;
  }
  return i;
}

//create an array of addresses
ADDR* initAddrList(int length){
  ADDR* result = calloc(length + 1, sizeof(ADDR)); //We calloc one more element, so that the last element of the array is an empty element
  assert(result != NULL);
  return (result);
}

//free an array of addresses
void freeAddrList(ADDR* tab){
  free(tab);
}

//read the file of addresses to fulfill our array
//it takes to arguments : the place where is the file and the maximal number of addresses it could have.
ADDR* addrGenerator(char* locate, int length){
  FILE * addrFile;
  ADDR * array = initAddrList(length);
  char * line = malloc(MAX_LEN_LINE_IN_FILE * sizeof(char));
  char * addrFull = NULL;
  char * ipOnly = NULL;
  char * rankStr = NULL;
  int rank;
  bool alreadyExist[length];
  int currentLine;
  int i;
  for (i = 0; i < length; i++) {
    alreadyExist[i] = false;
  }

  addrFile = fopen(locate, "r");
  if (addrFile == NULL )
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
        "Error opening file");
  else {
    currentLine = 0;
    //Delete the spaces at the beginning of the first line
    char blank;
    while ((blank = fgetc(addrFile)) == ' ') {
    };
    ungetc(blank, addrFile);

    while (fgets(line, MAX_LEN_LINE_IN_FILE, addrFile) != NULL ) {
      currentLine++;
      //Char storage for later tests
      int lineLength = strlen(line);
      char lastLineChar = line[lineLength - 1];

      if ((line[0] != '#') && (line[0] != '\n')) {
        rankStr = strtok(line, ":");
        rank = atoi(rankStr);
        ipOnly = strtok(NULL, ":");
        addrFull = strtok(NULL, ":\n #");

        // Error manager
        if (lastLineChar != '\n' || rank < 0 || rank >= 16 || ipOnly == NULL
            || addrFull == NULL || stringLength(ipOnly) >= MAX_LEN_IP
            || stringLength(addrFull) >= MAX_LEN_CHAN || alreadyExist[rank]) {

          char * errorType = malloc(64 * sizeof(char));
          if (lastLineChar != '\n') {
            sprintf(errorType, "LINE too long (should be at most %d char)",
                MAX_LEN_LINE_IN_FILE);
          } else if (rank < 0 || rank >= 16) {
            strcpy(errorType, "RANK error : should be between 0 and 15");
          } else if (ipOnly == NULL || addrFull == NULL ) {
            strcpy(errorType, "RANK:HOSTNAME:PORT Syntax error");
          } else if (stringLength(ipOnly) >= 64) {
            strcpy(errorType, "HOSTNAME too long");
          } else if (stringLength(addrFull) >= 64) {
            strcpy(errorType, "PORT too long");
          } else if (alreadyExist[rank]) {
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

        alreadyExist[rank] = true;

        // if localhost is detected in addr_file, we replace it with the actual hostname
        if (!strcmp(ipOnly, "localhost")) {
          char ipOnlyForHostname[64];
          i = gethostname(ipOnlyForHostname, 64);
          if (i < 0) {
            error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
                "Error getting hostname");
          }
          ipOnly = ipOnlyForHostname;
        }

        for (i = 0; i < length; i++) {
          if (!strcmp(array[i].ip, ipOnly)) {
            if (!strcmp(array[i].chan, addrFull)) {
              error_at_line(EXIT_FAILURE, 0, __FILE__, __LINE__,
                  "\n%s:%d: This participant already exists in a previous line\n"
                      "Each hostname:port pair should be unique\n", locate,
                  currentLine);
            }
          }
        }
        strcpy(array[rank].ip, ipOnly);
        strcpy(array[rank].chan, addrFull);
      }
      //Delete the spaces at the beginning of the next line
      while ((blank = fgetc(addrFile)) == ' ')
        ;
      ungetc(blank, addrFile);

    }

  }
  fclose(addrFile);

  //Place a fake participant in empty cells of the array to fulfill it
  for (i = 0; i < NP; i++) {
    if (!alreadyExist[i]) {
      strcpy(array[i].ip, "localhost");
      strcpy(array[i].chan, "3000");
    }
  }

  free(line);
  return (array);
}

//add a tcomm to a place in an ADDR*
void addTComm(t_comm * tcomm, int i, ADDR * array, bool isPred){
  if (array[i].tcomm[0] == NULL ) {
    array[i].tcomm[0] = tcomm;
    array[i].isPred[0] = isPred;
  } else {
    array[i].tcomm[1] = tcomm;
    array[i].isPred[1] = isPred;
  }
}

//Tries to return a non-NULL t_comm at place i in an ADDR*
t_comm *getTComm(int i, bool isPred, ADDR * array){
  if ((array[i].tcomm[0] != NULL )&& (array[i].isPred[0] == isPred)){
  return array[i].tcomm[0];
} else if (array[i].isPred[1] == isPred) {
  return array[i].tcomm[1];
} else {
  return NULL;
}
}

//remove a tcomm from a place in an ADDR*
void removeTComm(t_comm * tcomm, int i, ADDR * array){
  if (array[i].tcomm[0] == tcomm) {
    array[i].tcomm[0] = NULL;
  } else if (array[i].tcomm[1] == tcomm) {
    array[i].tcomm[1] = NULL;
  }
}

//search a t_comm in an array
void searchTComm(t_comm * tcomm, ADDR * array, int *prank, bool *pisPred){
  int i = 0;

  *prank = -1;
  for (i = 0; i < NP; i++) {
    if (array[i].tcomm[0] == tcomm) {
      *prank = i;
      *pisPred = array[i].isPred[0];
      return;
    } else if (array[i].tcomm[1] == tcomm) {
      *prank = i;
      *pisPred = array[i].isPred[1];
      return;
    }
  }
}

//give the place in the array of a given address
//return -1 if it's unfound
int addrID(char * ip, char * chan, ADDR * array){
  int i = 0;

  while ((array[i].ip[0] != '\0')
      && (strcmp(array[i].ip, ip) != 0 || strcmp(array[i].chan, chan) != 0)) {
    i++;
  }

  if (array[i].ip[0] != '\0') {
    return (i);
  } else {
    return (-1);
  }
}
