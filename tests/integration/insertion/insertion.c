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
 Program for doing  insert duration tests
 Syntax:
 insert wagonMaxLen size interval participantNumber trainsNumber position
 Where :
 - delay is the delay before the insertion of this participant
 - participationDuration is the duration of its participation

 For the printf, we have used ";" as the separator between data.
 As explained in http://forthescience.org/blog/2009/04/16/change-separator-in-gnuplot/, to change
 the default separator " " in gnuplot, do:
 set datafile separator ";"
 */
#include <stdlib.h>
#include <stdio.h>

#ifdef INSERTION_TEST

#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <strings.h>
#include "trains.h"
#include "errorTrains.h"

#define TEST_MESSAGE FIRST_VALUE_AVAILABLE_FOR_MESS_TYP

int size, interval, position, participantNumber, trainsNumber;

static bool terminate = false;

void callbackCircuitChange(circuitView *cp){
  char s[MAX_LEN_ADDRESS_AS_STR];

  printf("!!! ******** callbackCircuitChange called with %d members (process ",
      cp->cv_nmemb);
  if (!addrIsNull(cp->cv_joined)) {
    printf("%s has arrived)\n", addrToStr(s, cp->cv_joined));
  } else {
    printf("%s is gone)\n", addrToStr(s, cp->cv_departed));
  }
}

void callbackODeliver(address sender, t_typ messageTyp, message *mp){

  if (payloadSize(mp) != size) {
    fprintf(stderr,
        "Error in file %s:%d : Payload size is incorrect: it is %zu when it should be %d\n",
        __FILE__, __LINE__, payloadSize(mp), size);
    exit(EXIT_FAILURE);
  }
}

void *timeKeeper(void *null){

  int participationDuration = 2 * (participantNumber - position) * interval
      + interval;

  usleep(participationDuration * 1000000);
  printf("%d seconds have past\nGame Over\n", participationDuration);
  terminate = true;
  return NULL ;
}

#endif /* INSERTION_TEST */

int main(int argc, char *argv[]){

#ifdef INSERTION_TEST

  int rc;
  int rankMessage = 0;
  pthread_t timeKeeperThread;

  if (argc != 7) {
    printf("%s wagonMaxLen size interval participantNumber trainsNumber position\n", argv[0]);
    printf("\t- wagonMaxLen is the maximum size of wagons\n");
    printf("\t- size is the size of messages (should be more than %lu)\n", sizeof(int));
    printf("\t- interval is the time (in seconds) between each event during the experience\n");
    printf("\t- participantNumber is the number of participant during the experience\n");
    printf("\t- trainsNumber is the number of trains during the experience\n");
    printf("\t- position is the position of the participant during the experience (first one has position number 1, NOT 0)\n");
    return EXIT_FAILURE;
  }

  // We initialize the different variables which will be used
  wagonMaxLen = atoi(argv[1]);
  size = atoi(argv[2]);
  if (size < sizeof(int)){
    printf("size should be more than sizeof(int) = %zu\n", sizeof(int));
  }
  interval = atoi(argv[3]);
  participantNumber = atoi(argv[4]);
  trainsNumber = atoi(argv[5]);
  position = atoi(argv[6]);

  /* We wait a number of sec between insertion */
  int delay = (position - 1) * interval;
  if (delay > 0) {
    printf("%d sec before insertion\n", delay);
    usleep(delay * 1000000);
  }

  // We create the timeKeeper thread
  rc = pthread_create(&timeKeeperThread, NULL, timeKeeper, NULL );
  if (rc < 0)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  rc = pthread_detach(timeKeeperThread);
  if (rc < 0)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");

  // We initialize the trains protocol
  rc = trInit(trainsNumber, wagonMaxLen, 0, 0, callbackCircuitChange, callbackODeliver, UNIFORM_TOTAL_ORDER);
  if (rc < 0) {
    trError_at_line(rc, trErrno, __FILE__, __LINE__, "trInit()");
    return EXIT_FAILURE;
  }

  // Process sends messages
  while (!terminate) {
    message *mp = newmsg(size);
    if (mp == NULL ) {
      trError_at_line(rc, trErrno, __FILE__, __LINE__, "newmsg()");
      return EXIT_FAILURE;
    }
    rankMessage++;
    *((int*) (mp->payload)) = rankMessage;
    if (oBroadcast(TEST_MESSAGE, mp) < 0) {
      trError_at_line(rc, trErrno, __FILE__, __LINE__, "oBroadcast()");
      return EXIT_FAILURE;
    }
  }

  rc = trTerminate();
  if (rc < 0) {
    trError_at_line(rc, trErrno, __FILE__, __LINE__, "trInit()");
    return EXIT_FAILURE;
  }

#else /* INSERTION_TEST */

  printf("To run insertion duration tests, you have to compile with the tests target\n");

#endif /* INSERTION_TEST */

  return EXIT_SUCCESS;
}

