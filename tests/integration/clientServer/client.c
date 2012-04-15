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

int main(int argc, char *argv[]) {
  t_comm *commForConnect;
  message *msg;
  int len;

  if (argc != 3){
    fprintf(stderr, "USAGE = %s hostname port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // We open a connection to send messages
  printf("Connecting %s on port %s...\n", argv[1], argv[2]);
  commForConnect = comm_newAndConnect(argv[1], argv[2], CONNECT_TIMEOUT);
  if (commForConnect == NULL)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newAndConnect");
  printf("...OK\n");

  len = sizeof(((message*)NULL)->len)+strlen(HW)+sizeof('\0');
  msg = malloc(len);
  assert(msg != NULL);
  msg->len = len;
  strcpy(msg->payload, HW);
  printf("\tSend message of %d bytes with: \"%s\"...\n", len, HW);
  comm_write(commForConnect, msg, msg->len);
  free(msg);

  len = sizeof(((message*)NULL)->len)+strlen(LONG_MESSAGE)+sizeof('\0');
  msg = malloc(len);
  assert(msg != NULL);
  msg->len = len;
  strcpy(msg->payload, LONG_MESSAGE);
  printf("\tSend message of %d bytes with: \"%s\"...\n", len, LONG_MESSAGE);
  comm_write(commForConnect, msg, msg->len);
  free(msg);

  // We sleep a little to give time to the message to arrive before closing 
  // connection
  // NB : the usleep is specific to this integration test! It is not necessary 
  //      in a standard application.
  usleep(100000);
  printf("Close connection...\n");
  comm_free(commForConnect);
  printf("...OK\n");

  return EXIT_SUCCESS;
}

  
