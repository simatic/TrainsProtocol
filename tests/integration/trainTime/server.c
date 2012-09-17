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

 Syntax = server port
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

void *connectionMgt(void *arg){
  trComm *aComm = (trComm*) arg;
  message *msg;
  int nbRead;
  struct timeval debut, fin, duree;
  struct rusage debutCPU, finCPU, dureeCPU;

  printf("\tNew connection\n");
  do {
    int len;
    nbRead = commReadFully(aComm, &len, sizeof(len));
    if (nbRead == sizeof(len)) {
      msg = malloc(len);
      assert(msg != NULL);
      msg->header.len = len;
      nbRead = commReadFully(aComm, &(msg->header.typ),
          msg->header.len - sizeof(len));
      if (nbRead == msg->header.len - sizeof(len)) {
        switch(msg->header.typ){
        case GO:
          //Démarrer les timers (gettimeofday et getrusage)
          printf("Received GO\n");
          getrusage(RUSAGE_SELF, &debutCPU);
          gettimeofday(&debut, NULL);
          break;

        case FAKE_TRAIN:
          break;

        case STOP:
          //arrêter les timers, puis print les resultats
          getrusage(RUSAGE_SELF, &finCPU);
          gettimeofday(&fin, NULL);
          timersub(&(finCPU.ru_utime), &(debutCPU.ru_utime), &(dureeCPU.ru_utime));
          timersub(&(finCPU.ru_stime), &(debutCPU.ru_stime), &(dureeCPU.ru_stime));
          timersub(&fin, &debut, &duree);

          printf("Temps absolu écoulé :          %9ld usec\n", duree.tv_usec);
          printf("Temps CPU (user+sys) écoulé :  %9ld usec\n",
                  dureeCPU.ru_utime.tv_usec + dureeCPU.ru_stime.tv_usec);

          printf("Received STOP of %7d bytes\n\n", msg->header.len);
          break;

        case WRITE_PHASE:
          printf("******************** WRITE_PHASE ********************\n"
              "*****************************************************\n");
          break;

        case WRITE_V_PHASE:
          printf("******************** WRITE_V_PHASE ******************\n"
              "*****************************************************\n");
          break;

        default:
          printf("Received unknown type message : %d\n", msg->header.typ);
          break;

        }

      } else {
        printf(
            "\t\t...Received only %d/%lu bytes ",
            nbRead, msg->header.len - sizeof(len));
      }
      free(msg);
    } else if (nbRead > 0) {
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
          "Read only %d/%lu bytes\n", nbRead, sizeof(len));
    }
  } while (nbRead > 0);

  freeComm(aComm);

  if (nbRead == 0) {
    printf("\t...Connection has been closed\n");
  } else if (errno == EINTR) {
    printf("\t...commReadFully was aborted\n");
  } else
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "commReadFully");

  return NULL ;
}

int main(int argc, char *argv[]){
  trComm *commForAccept;
  trComm *aComm;

  if (argc != 2) {
    fprintf(stderr, "USAGE = %s port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  printf("Accepting connections on port %s...\n", argv[1]);
  commForAccept = commNewForAccept(argv[1]);
  if (commForAccept == NULL )
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newForAccept");

  do {
    aComm = commAccept(commForAccept);
    if (aComm != NULL ) {
      // We fork a thread responsible for handling this connection
      pthread_t thread;
      int rc = pthread_create(&thread, NULL, &connectionMgt, (void *) aComm);
      if (rc < 0)
        error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
      rc = pthread_detach(thread);
      if (rc < 0)
        error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
    }
  } while (aComm != NULL );

  if (errno == EINTR) {
    printf("\t...comm_accept was aborted\n");
    freeComm(commForAccept);
  } else
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_accept");

  return EXIT_SUCCESS;
}

