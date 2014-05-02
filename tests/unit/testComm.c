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

/* Test of the Comm module */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "comm.h"
#include "common.h" // To have the boolean type :p
#include "trains.h" // To have message typedef
#include "errorTrains.h"

#define CONNECT_TIMEOUT 2000 // milliseconds
#define LOCAL_HOST "localhost"
#define REMOTE_EXISTING "ssh.it-sudparis.eu"
#define REMOTE_NON_EXISTING "192.168.255.254"
#define PORT "4242"
#define PORT_NON_EXISTING "44444"

#define HW "Hello world!"
#define LONG_MESSAGE "This is a long message: 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define AVERAGE_SIZE 32 //This default value is estimated by considering the average size of received message

bool full_or_not=false; // To say if commReadFully is used instead of commRead

void *connectionMgt(void *arg) {
  trComm *aComm = (trComm*)arg;
  message *msg;
  int nbRead;

  do{
    int len;
    if (full_or_not==false){
      nbRead = commRead(aComm, &len, sizeof(len));
      if (nbRead > 0){
	msg = malloc(len);
	assert(msg != NULL);
	msg->header.len=len;
	nbRead  = commRead(aComm, ((char*)msg)+nbRead, msg->header.len - nbRead);
	printf("\t...Received message of %d bytes with: \"%s\"\n", msg->header.len, msg->payload);
	free(msg);
      }
    }
    else {
      nbRead = commReadFully(aComm, &len, sizeof(len));
      if (nbRead > 0){
        msg = malloc(len);
        assert(msg != NULL);
        msg->header.len=len;
        nbRead  = commReadFully(aComm, ((char*)msg)+nbRead, msg->header.len - nbRead);
        printf("\t...Received message of %d bytes with: \"%s\"\n", msg->header.len, msg->payload);
        free(msg);
      }
    }
  } while (nbRead > 0);

  if (nbRead == 0){
    printf("\t...Connection has been closed\n");
    freeComm(aComm);
  } else if (errno == EINTR){
    printf("\t...comm_read was aborted\n");
    // In this test, commRead was aborted because aComm was freed. So
    // there is no need to call: freeComm(aComm);
  } else
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_read");

  return NULL;
}

void *connectionMgt2(void *arg) {
  trComm *aComm = (trComm*)arg;
  int nbRead;
  char msg;

  if (full_or_not==false){
    nbRead = commRead(aComm, &msg, sizeof(msg));
   } else {
    nbRead = commReadFully(aComm, &msg, sizeof(msg));
  }
  if (nbRead != 0){
    printf("We read something else than 0 bytes (nbRead = %d)\n", nbRead);
    abort();
  }
  printf("\t...comm_read was aborted\n");

  return NULL;
}

// Thread taking care of commAccept for standard connection tests
void *acceptMgt(void *arg) {
  trComm *commForAccept = (trComm*)arg;
  trComm *aComm;

  do{
    aComm = commAccept(commForAccept);
    if (aComm != NULL){
      // We fork a thread responsible for handling this connection
      pthread_t thread;
      int rc = pthread_create(&thread, NULL, connectionMgt, (void *)aComm);
      if (rc < 0)
	ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
      rc = pthread_detach(thread);
      if (rc < 0)
	ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
    }
  } while (aComm != NULL);

  if (errno == EINVAL){
    printf("\t...comm_accept was aborted\n");
  }else
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_accept");

  return NULL;
}

// Thread taking care of commAccept for testing that freeComm() works 
// correctly with a thread blocked on commRead()
void *acceptMgt2(void *arg) {
  trComm *commForAccept = (trComm*)arg;
  trComm *aComm;
  pthread_t thread;
  int rc;

  aComm = commAccept(commForAccept);
  if (aComm != NULL){
    // We fork a thread responsible for handling this connection
    // Note: We must use a connectionMgt2 because it must not apply
    //       freeComm() on a aComm
    rc = pthread_create(&thread, NULL, connectionMgt2, (void *)aComm);
    if (rc < 0)
      ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  }else
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_accept");
    
  // We sleep a little to let the connectionMgt thread start
  // NB : this sleep is specific to this unit test! It is not necessary 
  //      in a standard application.
  usleep(10000);

  printf("\tBegin  comm_free with a thread blocked in comm_read()...\n");
  freeComm(aComm);
  rc = pthread_join(thread, NULL);
  if(rc)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_join");

  printf("\t......End comm_free with a thread blocked in comm_read().\n");

  freeComm(commForAccept);

  return NULL;
}

int main() {
  trComm *commForAccept;
  trComm *commForConnect;
  int rc;
  pthread_t thread;
  message *msg;
  int len;
  char answer;

  printf("testing comm...\n");
  puts("Would you like to test with comm_readFully (Y/n)");
  answer=getchar();
  full_or_not = (answer=='Y'||answer=='y');

  commForAccept = commNewForAccept(PORT);
  if (commForAccept == NULL)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newForAccept");

  rc = pthread_create(&thread, NULL, acceptMgt, (void *)commForAccept);
  if (rc < 0)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");

  // We sleep a little to let the acceptMgt thread start
  // NB : this sleep is specific to this unit test! It is not necessary 
  //      in a standard application.
  usleep(10000);

  // We open a connection to send messages
  commForConnect = commNewAndConnect(LOCAL_HOST, PORT, CONNECT_TIMEOUT);
  if (commForConnect == NULL)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newAndConnect");

  len = sizeof(messageHeader)+strlen(HW)+1; //+1 for '\0'
  msg = malloc(len);
  assert(msg != NULL);
  msg->header.len = len;
  msg->header.typ = 0;
  strcpy(msg->payload, HW);
  printf("\tSend message of %d bytes with: \"%s\"...\n", len, HW);
  commWrite(commForConnect, msg, msg->header.len);
  free(msg);

  len = sizeof(messageHeader)+strlen(LONG_MESSAGE)+1; //+1 for '\0'
  msg = malloc(len);
  assert(msg != NULL);
  msg->header.len = len;
  msg->header.typ = 0;
  strcpy(msg->payload, LONG_MESSAGE);
  printf("\tSend message of %d bytes with: \"%s\"...\n", len, LONG_MESSAGE);
  commWrite(commForConnect, msg, msg->header.len);
  free(msg);

  // We sleep a little to give time to the message to arrive before closing 
  // connection
  // NB : the usleep is specific to this unit test! It is not necessary 
  //      in a standard application.
  usleep(10000);
  printf("\tClose connection...\n");
  freeComm(commForConnect);
  usleep(10000);

  // We try to connect to sites that refuse the connection
  printf("\tTesting user-specified connect timeout when connecting to an existing site (%s) which does not accept this port...\n", REMOTE_EXISTING);
  commForConnect = commNewAndConnect(REMOTE_EXISTING, PORT, CONNECT_TIMEOUT);
  if (commForConnect == NULL){
    if (errno == ENETUNREACH) 
      printf("\t...OK\n");
    else
      ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newAndConnect");
  }

  printf("\tTesting user-specified connect timeout when connecting to an non-existing site (%s) which does not accept this port...\n", REMOTE_NON_EXISTING);
  commForConnect = commNewAndConnect(REMOTE_NON_EXISTING, PORT, CONNECT_TIMEOUT);
  if (commForConnect == NULL){
    if (errno == ETIMEDOUT) 
      printf("\t...OK\n");
    else
      ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newAndConnect");
  }

  printf("\tTesting user-specified connect timeout when connecting to localhost with non-existing port (%s)...\n", PORT_NON_EXISTING);
  commForConnect = commNewAndConnect(LOCAL_HOST, PORT_NON_EXISTING, CONNECT_TIMEOUT);
  if (commForConnect == NULL){
    if (errno == ECONNREFUSED) 
      printf("\t...OK\n");
    else
      ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newAndConnect");
  }

  // We free commForAccept to check that it shutdowns the socket, so that
  // the accept is stopped.
  printf("\tfreeComm commForAccept...\n");
  freeComm(commForAccept);

  rc = pthread_join(thread, NULL);
  if(rc)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_join");

  //
  // Now we create a new socket to check that freeComm() works correctly
  // with a thread blocked on commRead()
  //
  commForAccept = commNewForAccept(PORT);
  if (commForAccept == NULL)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newForAccept");

  rc = pthread_create(&thread, NULL, acceptMgt2, (void *)commForAccept);
  if (rc < 0)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");

  // We open a connection so that the acceptMgt2 thread creates a thread
  // to handle the connection
  commForConnect = commNewAndConnect(LOCAL_HOST, PORT, CONNECT_TIMEOUT);
  if (commForConnect == NULL)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newAndConnect");

  rc = pthread_join(thread, NULL);
  if(rc)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_join");

  freeComm(commForConnect);

  // free memory 

  printf("...OK\n");

  return EXIT_SUCCESS;
}

  
