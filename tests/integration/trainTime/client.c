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

/* Client of a client-server application designed to check 
 that Comm module works correctly between different machines

 Syntax = client hostname port
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "comm.h"
#include "trains.h" // To have message typedef
#include "trainTime.h"

#define CONNECT_TIMEOUT 2000 // milliseconds
#define HW "Hello world!"
#define LONG_MESSAGE "This is a long message: 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define VERY_LONG_MESSAGE_SIZE 1000000


int main(int argc, char *argv[]){
  trComm *commForConnect;
  message *msg;
  int len, nbWritten;
  int sizeArray[] = { 20, 100, 200, 500, 1000, 2000, 5000, 10000, 15000, 20000 };
  int i, j;
  struct timeval debut, fin, duree;
  struct rusage debutCPU, finCPU, dureeCPU;
  struct iovec iov[3];

  if (argc != 3) {
    fprintf(stderr, "USAGE = %s hostname port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // We open a connection to send messages
  printf("Connecting %s on port %s...\n", argv[1], argv[2]);
  commForConnect = commNewAndConnect(argv[1], argv[2], CONNECT_TIMEOUT);
  if (commForConnect == NULL )
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
        "comm_newAndConnect");
  printf("...OK\n");

  /*******************    WRITE PHASE START   **************************/

  len = sizeof(messageHeader) + 1;
  msg = malloc(len);
  assert(msg != NULL);
  msg->header.len = len;
  msg->header.typ = WRITE_PHASE;
  printf("\nSending *********** %s *********** message of %d bytes\n",
      msgTypeToStr(msg->header.typ), len);
  nbWritten = commWrite(commForConnect, msg, msg->header.len);
  if (nbWritten != len) {
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
        "sent only %d/%d bytes", nbWritten, len);
  }
  free(msg);

  usleep(1000000);

  /* ******************  WRITE LOOP   ********************************/

  for (i = 0; i < 10; i++) {

    len = sizeof(messageHeader) + 1;
    msg = malloc(len);
    assert(msg != NULL);
    msg->header.len = len;
    msg->header.typ = GO;
    printf("\nSending *********** %s *********** message of %d bytes\n",
        msgTypeToStr(msg->header.typ), len);
    nbWritten = commWrite(commForConnect, msg, msg->header.len);
    if (nbWritten != len) {
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
          "sent only %d/%d bytes", nbWritten, len);
    }
    free(msg);

    getrusage(RUSAGE_SELF, &debutCPU);
    gettimeofday(&debut, NULL );

    /* *************************** FAKE TRAINS LOOP ****************/

    for (j = 0; j < 1000; j++) {

      len = sizeof(messageHeader) + sizeArray[i];
      msg = malloc(len);
      assert(msg != NULL);
      msg->header.len = len;
      msg->header.typ = FAKE_TRAIN;
      nbWritten = commWrite(commForConnect, msg, msg->header.len);
      if (nbWritten != len) {
        error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
            "sent only %d/%d bytes", nbWritten, len);
      }
    }

    /* ****************************** MESSAGE STOP  *******************/

    len = sizeof(messageHeader) + sizeArray[i];
    msg = malloc(len);
    assert(msg != NULL);
    msg->header.len = len;
    msg->header.typ = STOP;
    nbWritten = commWrite(commForConnect, msg, msg->header.len);
    if (nbWritten != len) {
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
          "sent only %d/%d bytes", nbWritten, len);
    }

    /***************************** CALCULATION ************************/

    getrusage(RUSAGE_SELF, &finCPU);
    gettimeofday(&fin, NULL );
    timersub(&(finCPU.ru_utime), &(debutCPU.ru_utime), &(dureeCPU.ru_utime));
    timersub(&(finCPU.ru_stime), &(debutCPU.ru_stime), &(dureeCPU.ru_stime));
    timersub(&fin, &debut, &duree);

    printf("Temps absolu écoulé :          %9ld usec\n", duree.tv_usec);
    printf("Temps CPU (user+sys) écoulé :  %9ld usec\n",
        dureeCPU.ru_utime.tv_usec + dureeCPU.ru_stime.tv_usec);

    printf("Sent *********** %s *********** message of %d bytes\n",
        msgTypeToStr(msg->header.typ), len);

    free(msg);

    usleep(1000000);

  }

  len = sizeof(messageHeader) + 1;
  msg = malloc(len);
  assert(msg != NULL);
  msg->header.len = len;
  msg->header.typ = WRITE_V_PHASE;
  printf("\tSending %s message of %d bytes\n", msgTypeToStr(msg->header.typ),
      len);
  nbWritten = commWrite(commForConnect, msg, msg->header.len);
  if (nbWritten != len) {
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
        "sent only %d/%d bytes", nbWritten, len);
  }
  free(msg);

  /**************************************************************************
   * ************************************************************************
   * ************************************************************************
   * ************************************************************************
   * ************************************************************************/

  usleep(1000000);

  /* ******************  WRITE LOOP   ********************************/

  for (i = 0; i < 10; i++) {

    len = sizeof(messageHeader) + 1;
    msg = malloc(len);
    assert(msg != NULL);
    msg->header.len = len;
    msg->header.typ = GO;
    printf("\nSending *********** %s *********** message of %d bytes\n",
        msgTypeToStr(msg->header.typ), len);
    nbWritten = commWrite(commForConnect, msg, msg->header.len);
    if (nbWritten != len) {
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
          "sent only %d/%d bytes", nbWritten, len);
    }
    free(msg);

    getrusage(RUSAGE_SELF, &debutCPU);
    gettimeofday(&debut, NULL );

    /* *************************** FAKE TRAINS LOOP ****************/

    for (j = 0; j < 1000; j++) {

      len = sizeof(messageHeader) + sizeArray[i];
      msg = malloc(len);
      assert(msg != NULL);
      msg->header.len = len;
      msg->header.typ = FAKE_TRAIN;

      iov[0].iov_base = msg;
      iov[0].iov_len = sizeof(messageHeader);

      iov[1].iov_base = &(msg->payload[0]);
      iov[1].iov_len = 4 * sizeArray[i] / 5;

      iov[2].iov_base = &(msg->payload[4 * sizeArray[i] / 5]);
      iov[2].iov_len = sizeArray[i] / 5;

      /*
      for (k = 0; k < NB_CHUNKS; k++) {
        iov[k + 1].iov_base = &(msg->payload[k * sizeArray[i] / NB_CHUNKS]);
        iov[k + 1].iov_len = sizeArray[i] / NB_CHUNKS;
      }*/
      nbWritten = commWritev(commForConnect, iov, 3);

      if (nbWritten != len) {
        error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
            "sent only %d/%d bytes", nbWritten, len);
      }
    }

    /* ****************************** MESSAGE STOP  *******************/

    len = sizeof(messageHeader) + sizeArray[i];
    msg = malloc(len);
    assert(msg != NULL);
    msg->header.len = len;
    msg->header.typ = STOP;

    iov[0].iov_base = msg;
    iov[0].iov_len = sizeof(messageHeader);

    iov[1].iov_base = &(msg->payload[0]);
    iov[1].iov_len = 4 * sizeArray[i] / 5;

    iov[2].iov_base = &(msg->payload[4 * sizeArray[i] / 5]);
    iov[2].iov_len = sizeArray[i] / 5;



    /*for (k = 0; k < NB_CHUNKS; k++) {
      iov[k + 1].iov_base = &(msg->payload[k * sizeArray[i] / NB_CHUNKS]);
      iov[k + 1].iov_len = sizeArray[i] / NB_CHUNKS;
    }*/
    nbWritten = commWritev(commForConnect, iov, 3);
    if (nbWritten != len) {
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
          "sent only %d/%d bytes", nbWritten, len);
    }

    /***************************** CALCULATION ************************/

    getrusage(RUSAGE_SELF, &finCPU);
    gettimeofday(&fin, NULL );
    timersub(&(finCPU.ru_utime), &(debutCPU.ru_utime), &(dureeCPU.ru_utime));
    timersub(&(finCPU.ru_stime), &(debutCPU.ru_stime), &(dureeCPU.ru_stime));
    timersub(&fin, &debut, &duree);

    printf("Temps absolu écoulé :          %9ld usec\n", duree.tv_usec);
    printf("Temps CPU (user+sys) écoulé :  %9ld usec\n",
            dureeCPU.ru_utime.tv_usec + dureeCPU.ru_stime.tv_usec);

    printf("Sent *********** %s *********** message of %d bytes\n",
        msgTypeToStr(msg->header.typ), len);

    free(msg);

    usleep(1000000);

  }


  printf("******************** END OF TRANSMISSION *************************\n");

  usleep(1000000);
  printf("Close connection...\n");
  freeComm(commForConnect);
  printf("...OK\n");

  return EXIT_SUCCESS;
}

