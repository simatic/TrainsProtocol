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
  int i;

  printf("\tNew connection\n");
  do{
    int len;
    nbRead = comm_readFully(aComm, &len, sizeof(len));
    if (nbRead == sizeof(len)){
      msg = malloc(len);
      assert(msg != NULL);
      msg->header.len=len;
      nbRead  = comm_readFully(aComm, &(msg->header.typ), msg->header.len - sizeof(len));
      if (nbRead == msg->header.len - sizeof(len)){
	if (msg->header.len < 1000) {
	  printf("\t\t...Received message of %d bytes with: \"%s\"\n", msg->header.len, msg->payload);
	} else {
	  printf("\t\t...Received message of %d bytes ", msg->header.len);
	  // Check contents
	  for(i=0; i<msg->header.len - sizeof(message_header); i++){
	    if ((unsigned char)(msg->payload[i]) != i%256){
	      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "Received long message which contents is incorrect (at %d-th position, found %d instead of %d)\n", i, (unsigned char)(msg->payload[i]), i%256);
	    }
	  }
	  printf("and contents is OK\n");
	}
      } else {
	printf("\t\t...Received only %d/%d bytes (but this could be normal, as the client may have sent on purpose an incomplete message)\n", nbRead, msg->header.len - sizeof(len));
      }
      free(msg);
    } else if (nbRead > 0) {
	error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "Read only %d/%d bytes\n", nbRead, sizeof(len));
    }
  } while (nbRead > 0);

  comm_free(aComm);

  if (nbRead == 0){
    printf("\t...Connection has been closed\n");
  } else if (errno == EINTR){
    printf("\t...comm_readFully was aborted\n");
  } else
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_readFully");

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

  
