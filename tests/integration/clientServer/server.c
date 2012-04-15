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

#include "comm.h"
#include "trains.h" // To have message typedef

#define AVERAGE_SIZE 32 //This default value is estimated by considering the average size of received message

void *connectionMgt(void *arg) {
  t_comm *aComm = (t_comm*)arg;
  message *msg;
  int nbRead;

  printf("\tNew connection\n");
  do{
    int len;
    nbRead = comm_read(aComm, &len, sizeof(len));
    if (nbRead > 0){
      msg = malloc(len);
      assert(msg != NULL);
      msg->len=len;
      nbRead  = comm_read(aComm, msg->payload, msg->len - nbRead);
      printf("\t\t...Received message of %d bytes with: \"%s\"\n", msg->len, msg->payload);
      free(msg);
    }
  } while (nbRead > 0);

  comm_free(aComm);

  if (nbRead == 0){
    printf("\t...Connection has been closed\n");
  } else if (errno == EINTR){
    printf("\t...comm_read was aborted\n");
  } else
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_read");

  return NULL;
}

int main(int argc, char *argv[]) {
  t_comm *commForAccept;
  t_comm *aComm;

  if (argc != 2){
    fprintf(stderr, "USAGE = %s port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  printf("Accepting connections on port %s...\n", argv[1]);
  commForAccept = comm_newForAccept(argv[1]);
  if (commForAccept == NULL)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newForAccept");

  do{
    aComm = comm_accept(commForAccept);
    if (aComm != NULL){
      // We fork a thread responsible for handling this connection
      pthread_t thread;
      int rc = pthread_create(&thread, NULL, &connectionMgt, (void *)aComm);
      if (rc < 0)
	error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
      rc = pthread_detach(thread);
      if (rc < 0)
	error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
    }
  } while (aComm != NULL);

  if (errno == EINTR){
    printf("\t...comm_accept was aborted\n");
    comm_free(commForAccept);
  }else
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_accept");

  return EXIT_SUCCESS;
}

  
