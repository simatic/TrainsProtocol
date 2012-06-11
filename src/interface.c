#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <error.h>

#include "interface.h"
#include "management_addr.h"
#include "iomsg.h"
#include "stateMachine.h"

sem_t sem_init_done;

int tr_errno;

int tr_init(CallbackCircuitChange callbackCircuitChange, CallbackUtoDeliver callbackUtoDeliver){
  int rc;
  pthread_t thread;

  rc = sem_init(&sem_init_done,0,0);
  if(rc)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "sem_init");

  rc= pthread_cond_init(&condWagonToSend, NULL);
  assert(rc == 0);

  theCallbackCircuitChange = callbackCircuitChange;
  theCallbackUtoDeliver = callbackUtoDeliver;

  global_addr_array = addr_generator(LOCALISATION, NP);
  my_address=rank_2_addr(addr_id(LOCAL_HOST,PORT,global_addr_array));

  automatonInit();
  do {
    rc = sem_wait(&sem_init_done);
  } while ((rc < 0) && (errno == EINTR));
  if (rc)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "sem_wait()");

  rc = pthread_create(&thread, NULL, uto_deliveries, NULL);
  if (rc)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  rc = pthread_detach(thread);
  if (rc)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");

  return 0;
}

void tr_error_at_line(int status, int errnum, const char *filename, unsigned int linenum, const char *format){
  fflush(stdout);
  fprintf(stderr, "basic version of tr_error_at_line\n");
}

void tr_perror(int errnum){
  fprintf(stderr, "basic version of tr_perror");
}

int tr_terminate(){
  return 0;
}
