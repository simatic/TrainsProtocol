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

/*
 Basic test program
 Syntax:
   basicTest sender nbMemberMin delayBetweenUtoBroadcast nbRecMsgBeforeStop
 Where:
   - sender is 'y' or 'Y' if user wants the process to utoBrodcast messages
   - nbMemberMin is the minimum number of members in the protocol before starting to utoBroadcast
   - delayBetweenUtoBroadcast is the minimum delay in microseconds between 2 utoBroadcasts by the same process
   - nbRecMsgBeforeStop is the minimum numer of messages to be received before process stops
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <semaphore.h>
#include <strings.h>
#include "trains.h"

#define PAYLOAD_SIZE sizeof(int)

sem_t semWaitEnoughMembers;
sem_t semWaitToDie;

bool sender;
int  nbMemberMin;
int  delayBetweenTwoUtoBroadcast;
int  nbRecMsgBeforeStop;

bool terminate = false;

void callbackCircuitChange(circuitView *cp){
  char s[MAX_LEN_ADDRESS_AS_STR];

  printf("!!! ******** callbackCircuitChange called with %d members (process ", cp->cv_nmemb);
  if (!addrIsNull(cp->cv_joined)){
    printf("%s has arrived)\n", addrToStr(s,cp->cv_joined));
  }else{
    printf("%s is gone)\n", addrToStr(s,cp->cv_departed));
  }

  if(cp->cv_nmemb >= nbMemberMin){
    printf("!!! ******** enough members to start utoBroadcasting\n");
    int rc = sem_post(&semWaitEnoughMembers);
    if (rc){
      error_at_line(rc, errno, __FILE__, __LINE__, "sem_post()");
      exit(EXIT_FAILURE);
    }
  }
}

void callbackUtoDeliver(address sender, message *mp){
  char s[MAX_LEN_ADDRESS_AS_STR];
  static int nbRecMsg = 0;

  if (payloadSize(mp) != PAYLOAD_SIZE){
    fprintf(stderr, "Error in file %s:%d : Payload size is incorrect: it is %d when it should be %d\n", 
	    __FILE__,
	    __LINE__,
	    payloadSize(mp),
	    PAYLOAD_SIZE);
    exit(EXIT_FAILURE);
  }

  nbRecMsg++;
  if (nbRecMsg >= nbRecMsgBeforeStop){
    terminate = true;
    if (sem_post(&semWaitToDie) < 0){
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "sem_post()");
    }
  }

  printf("!!! %5d-ieme message (recu de %s / contenu = %5d)\n", nbRecMsg, addrToStr(s,sender), *((int*)(mp->payload)));

}

int main(int argc, char *argv[]) {
  int rc;
  int  rankMessage = 0;

  if (argc != 5) {
    printf("%s sender nbMemberMin delayBetweenUtoBroadcast nbRecMsgBeforeStop\n", argv[0]);
    printf("\t- sender is 'y' or 'Y' if user wants the process to utoBrodcast messages\n");
    printf("\t- nbMemberMin is the minimum number of members in the protocol before starting to utoBroadcast\n");
    printf("\t- delayBetweenUtoBroadcast is the minimum delay in microseconds between 2 utoBroadcasts by the same process\n");
    printf("\t- nbRecMsgBeforeStop is the minimum numer of messages to be received before process stops\n");
    return EXIT_FAILURE;
  }

  // We initialize the different variables which will be used
  sender = (index("yY",argv[1][0]) != NULL);
  nbMemberMin = atoi(argv[2]);
  delayBetweenTwoUtoBroadcast = atoi(argv[3]);
  nbRecMsgBeforeStop = atoi(argv[4]);

  rc = sem_init(&semWaitEnoughMembers, 0, 0); 
  if (rc){
    error_at_line(rc, errno, __FILE__, __LINE__, "sem_init()");
    return EXIT_FAILURE;
  }

  rc = sem_init(&semWaitToDie, 0, 0); 
  if (rc){
    error_at_line(rc, errno, __FILE__, __LINE__, "sem_init()");
    return EXIT_FAILURE;
  }

  // We initialize the trains protocol
  rc = trInit(callbackCircuitChange, callbackUtoDeliver);
  if (rc < 0) {
    trError_at_line(rc, trErrno, __FILE__, __LINE__, "tr_init()");
    return EXIT_FAILURE;
  }

  do {
    rc = sem_wait(&semWaitEnoughMembers);
  } while ((rc < 0) && (errno == EINTR));
  if (rc){
    error_at_line(rc, errno, __FILE__, __LINE__, "sem_wait()");
    return EXIT_FAILURE;
  }

  // Process is member of the protocol
  if (sender){
    // Process sends messages
    while (!terminate){
      message *mp = newmsg(PAYLOAD_SIZE);
      if (mp == NULL){
	trError_at_line(rc, trErrno, __FILE__, __LINE__, "newmsg()");
	return EXIT_FAILURE;
      }    
      rankMessage++;
      *((int*)(mp->payload)) = rankMessage;
      if (utoBroadcast(mp) < 0){
	trError_at_line(rc, trErrno, __FILE__, __LINE__, "utoBroadcast()");
	return EXIT_FAILURE;
      }
      usleep(delayBetweenTwoUtoBroadcast);
    }
  }else{
    // Process waits that callbackUtoDelivery tells it to die
    do {
      rc = sem_wait(&semWaitToDie);
    } while ((rc < 0) && (errno == EINTR));
    if (rc){
      error_at_line(rc, errno, __FILE__, __LINE__, "sem_wait()");
      return EXIT_FAILURE;
    }
  }

  printf("Process received enough messages: Game over !\n");

  rc = trTerminate();
  if (rc < 0) {
    trError_at_line(rc, trErrno, __FILE__, __LINE__, "tr_init()");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}






