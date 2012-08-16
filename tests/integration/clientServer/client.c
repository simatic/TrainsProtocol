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

#include "comm.h"
#include "trains.h" // To have message typedef

#define CONNECT_TIMEOUT 2000 // milliseconds

#define HW "Hello world!"
#define LONG_MESSAGE "This is a long message: 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define VERY_LONG_MESSAGE_SIZE 1000000
#define NB_CHUNKS 4 //Number of chunks used to send message of VERY_LONG_MESSAGE_SIZE
#define IOVCNT (1 + NB_CHUNKS)

int main(int argc, char *argv[]) {
  trComm *commForConnect;
  message *msg;
  int len, lenIncomplete, nbWritten;
  struct iovec iov[IOVCNT];
  int i;

  if (argc != 3){
    fprintf(stderr, "USAGE = %s hostname port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // We open a connection to send messages
  printf("Connecting %s on port %s...\n", argv[1], argv[2]);
  commForConnect = commNewAndConnect(argv[1], argv[2], CONNECT_TIMEOUT);
  if (commForConnect == NULL)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newAndConnect");
  printf("...OK\n");

  len = sizeof(messageHeader)+strlen(HW)+sizeof('\0');
  msg = malloc(len);
  assert(msg != NULL);
  msg->header.len = len;
  strcpy(msg->payload, HW);
  printf("\tSend message of %d bytes with: \"%s\"...\n", len, HW);
  nbWritten = commWrite(commForConnect, msg, msg->header.len);
  if (nbWritten != len){
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "sent only %d/%d bytes", nbWritten, len);
  }
  free(msg);

  len = sizeof(messageHeader)+strlen(LONG_MESSAGE)+sizeof('\0');
  msg = malloc(len);
  assert(msg != NULL);
  msg->header.len = len;
  strcpy(msg->payload, LONG_MESSAGE);
  printf("\tSend message of %d bytes with: \"%s\"...\n", len, LONG_MESSAGE);
  nbWritten = commWrite(commForConnect, msg, msg->header.len);
  if (nbWritten != len){
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "sent only %d/%d bytes", nbWritten, len);
  }
  free(msg);

  // We send a very long message
  len = sizeof(messageHeader)+VERY_LONG_MESSAGE_SIZE;
  msg = malloc(len);
  assert(msg != NULL);
  msg->header.len = len;
  for(i=0; i<VERY_LONG_MESSAGE_SIZE; i++){
    msg->payload[i] = i%256;
  }
  printf("\tSend message of %d bytes\n", len);
  iov[0].iov_base = msg;
  iov[0].iov_len  = sizeof(messageHeader);
  for(i=0; i<NB_CHUNKS; i++){
    iov[i+1].iov_base = &(msg->payload[i*VERY_LONG_MESSAGE_SIZE/NB_CHUNKS]);
    iov[i+1].iov_len  = VERY_LONG_MESSAGE_SIZE/NB_CHUNKS;
  }
  nbWritten = commWritev(commForConnect, iov, IOVCNT);
  if (nbWritten != len){
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "sent only %d/%d bytes", nbWritten, len);
  }
  // We do not free msg yet, as we reuse it in the following lines
  //free(msg);

  // We send a very long message which is not complete
  lenIncomplete = len - VERY_LONG_MESSAGE_SIZE/NB_CHUNKS;
  printf("\tSend incomplete message of %d/%d bytes  ==> This should make the server unhappy\n", lenIncomplete, len);
  nbWritten = commWritev(commForConnect, iov, IOVCNT-1);
  if (nbWritten != lenIncomplete){
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "sent only %d/%d bytes", nbWritten, lenIncomplete);
  }
  free(msg);



  // We sleep a little to give time to the message to arrive before closing 
  // connection
  // NB : the usleep is specific to this integration test! It is not necessary 
  //      in a standard application.
  usleep(1000000);
  printf("Close connection...\n");
  freeComm(commForConnect);
  printf("...OK\n");

  return EXIT_SUCCESS;
}

  
