/*
 Mock module designed to run the basic test while only the layer
 of the middleware doing the interface with the application is ready
 */

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "trains.h"
#include "wagon.h"
#include "msg.h"
#include "advanced_struct.h"
#include "signalArrival.h"

int tr_errno = 0;

#define MOCK_RANK 0
#define MOCK_CIRCUIT 0x0003
extern int automatonState;

void *trainsSimulation(void *null){

  do {
    MUTEX_LOCK(mutexWagonToSend);

    bqueue_enqueue(wagonsToDeliver, wagonToSend);
    wagonToSend = newwiw();

    pthread_cond_signal(&condWagonToSend);
    MUTEX_UNLOCK(mutexWagonToSend);
    usleep(10000);
  } while(1);

  return NULL;
}


void tr_error_at_line(int status, int errnum, const char *filename, unsigned int linenum, const char *format){
  fflush(stdout);
  fprintf(stderr, "basic version of tr_error_at_line\n");
}

int tr_init(CallbackCircuitChange aCallbackCircuitChange, CallbackUtoDeliver aCallbackUtoDeliver){
  pthread_t thread;
  int rc;

  // Initializations which will appear in true tr_init
  rc= pthread_cond_init(&condWagonToSend, NULL);
  assert(rc == 0);
  wagonsToDeliver = bqueue_new();
  theCallbackCircuitChange = aCallbackCircuitChange;
  theCallbackUtoDeliver = aCallbackUtoDeliver;
  wagonToSend = newwiw();
  rc = pthread_create(&thread, NULL, uto_deliveries, NULL);
  if (rc)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  rc = pthread_detach(thread);
  if (rc)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
  my_address = rank_2_addr(MOCK_RANK); // In true tr_init, MOCK_RANK
                                       // must be replaced by computation
                                       // of the rank of the process

  // Initialisations specific to this mock (it simulates the software
  // layer taking care of trains protocol
  signalArrival(wagonToSend->p_wagon, my_address, MOCK_CIRCUIT);

  automatonState = 0x7FFFFFFF; // To be sure that we are not in ALONE state

  rc = pthread_create(&thread, NULL, trainsSimulation, NULL);
  if (rc)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  rc = pthread_detach(thread);
  if (rc)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");

  return 0;
}

void tr_perror(int errnum){
  fprintf(stderr, "basic version of tr_perror");
}

int tr_terminate(){
  return 0;
}
