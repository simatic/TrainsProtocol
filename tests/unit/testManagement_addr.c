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
#include <string.h>
#include "comm.h"
#include "management_addr.h"

#define NB_LINES_IN_FILE 16
#define FAKE_COMM1 ((t_comm*)0xFFFFFFFF)
#define FAKE_COMM2 ((t_comm*)0x8FFFFFFF)
#define FAKE_COMM3 ((t_comm*)0x88FFFFFF)
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
  int aRank;
  bool aIsPred;

  printf("taille = %d\n", sizeof(ADDR));
  globalAddrArray = addrGenerator(LOCALISATION, NB_LINES_IN_FILE);

  for (i=0; i<NB_LINES_IN_FILE; i++){
    memset(ip,65+i,MAX_LEN_IP);
    ip[MAX_LEN_IP-1] = '\0';
    memset(chan,97+i,MAX_LEN_CHAN);
    chan[MAX_LEN_CHAN-1] = '\0';
    compare("globalAddrArray contents", ((strcmp(ip,globalAddrArray[i].ip)==0) && (strcmp(chan,globalAddrArray[i].chan)==0) && (globalAddrArray[i].tcomm[0]==NULL) && (globalAddrArray[i].tcomm[1]==NULL)));
  }
  compare("globalAddrArray contents", ((globalAddrArray[i].ip[0]=='\0') && (globalAddrArray[i].chan[0]=='\0') && (globalAddrArray[i].tcomm[0]==NULL) && (globalAddrArray[i].tcomm[1]==NULL)));

  addTComm(FAKE_COMM1, 0, globalAddrArray, true);
  addTComm(FAKE_COMM2, 0, globalAddrArray, false);
  compare("addTComm", (globalAddrArray[0].tcomm[0]==FAKE_COMM1) && (globalAddrArray[0].isPred[0]==true));
  compare("addTComm", (globalAddrArray[0].tcomm[1]==FAKE_COMM2) && (globalAddrArray[0].isPred[1]==false));

  searchTComm(FAKE_COMM1, globalAddrArray, &aRank, &aIsPred);
  compare("searchTComm", (aRank==0) && (aIsPred==true));
  searchTComm(FAKE_COMM2, globalAddrArray, &aRank, &aIsPred);
  compare("searchTComm", (aRank==0) && (aIsPred==false));
  searchTComm(FAKE_COMM3, globalAddrArray, &aRank, &aIsPred);
  compare("searchTComm", (aRank==-1));

  compare("getTComm", getTComm(0,true,globalAddrArray)==FAKE_COMM1);
  compare("getTComm", getTComm(0,false,globalAddrArray)==FAKE_COMM2);
  removeTComm(FAKE_COMM1, 0, globalAddrArray);
  compare("get_tcomm", getTComm(0,true,globalAddrArray)==NULL);
  addTComm(FAKE_COMM1, 0, globalAddrArray, true);
  compare("get_tcomm", getTComm(0,true,globalAddrArray)==FAKE_COMM1);

  memset(ip,65+SEARCHED_INDEX,MAX_LEN_IP);
  ip[MAX_LEN_IP-1] = '\0';
  memset(chan,97+SEARCHED_INDEX,MAX_LEN_CHAN);
  chan[MAX_LEN_CHAN-1] = '\0';
  compare("addrID", addrID(ip, chan, globalAddrArray) == SEARCHED_INDEX);
  chan[0] = '\0';
  compare("addrID", addrID(ip, chan, globalAddrArray) == -1);

  removeTComm(FAKE_COMM2, 0, globalAddrArray);
  compare("removeTComm", globalAddrArray[0].tcomm[0]==FAKE_COMM1);
  compare("removeTComm", globalAddrArray[0].tcomm[1]==NULL);
  removeTComm(FAKE_COMM1, 0, globalAddrArray);
  compare("removeTComm", globalAddrArray[0].tcomm[0]==NULL);
  compare("removeTComm", globalAddrArray[0].tcomm[1]==NULL);


  freeAddrList(globalAddrArray);

  return EXIT_SUCCESS;
}
