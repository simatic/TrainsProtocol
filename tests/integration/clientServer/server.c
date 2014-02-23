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
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "errorTrains.h"
#include "comm.h"
#include "trains.h" // To have message typedef
#define AVERAGE_SIZE 32 //This default value is estimated by considering the average size of received message
void *connectionMgt(void *arg){
  trComm *aComm = (trComm*) arg;
  message *msg;
  int nbRead;
  int i;

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
        if (msg->header.len < 1000) {
          printf("\t\t...Received message of %d bytes with: \"%s\"\n",
              msg->header.len, msg->payload);
        } else {
          printf("\t\t...Received message of %d bytes ", msg->header.len);
          // Check contents
          for (i = 0; i < msg->header.len - sizeof(messageHeader); i++) {
            if ((unsigned char) (msg->payload[i]) != i % 256) {
              ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__,
                  "Received long message which contents is incorrect (at %d-th position, found %d instead of %d)\n",
                  i, (unsigned char) (msg->payload[i]), i % 256);
            }
          }
          printf("and contents is OK\n");
        }
      } else {
        printf(
            "\t\t...Received only %d/%zu bytes (but this could be normal, as the client may have sent on purpose an incomplete message)\n",
            nbRead, msg->header.len - sizeof(len));
      }
      free(msg);
    } else if (nbRead > 0) {
      ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__,
          "Read only %d/%zu bytes\n", nbRead, sizeof(len));
    }
  } while (nbRead > 0);

  freeComm(aComm);

  if (nbRead == 0) {
    printf("\t...Connection has been closed\n");
  } else if (errno == EINTR) {
    printf("\t...comm_readFully was aborted\n");
  } else
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_readFully");

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
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newForAccept");

  do {
    aComm = commAccept(commForAccept);
    if (aComm != NULL ) {
      // We fork a thread responsible for handling this connection
      pthread_t thread;
      int rc = pthread_create(&thread, NULL, &connectionMgt, (void *) aComm);
      if (rc < 0)
        ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
      rc = pthread_detach(thread);
      if (rc < 0)
        ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
    }
  } while (aComm != NULL );

  if (errno == EINTR) {
    printf("\t...comm_accept was aborted\n");
    freeComm(commForAccept);
  } else
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_accept");

  return EXIT_SUCCESS;
}

