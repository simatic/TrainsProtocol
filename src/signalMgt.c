#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "signalMgt.h"
#include "common.h"

pid_t signalMgtThreadTid = 0;

static sigset_t mask;
static pthread_t thread;

void comm_abort_whenIT();

/**
 * @brief Signal handler for SIGNAL_FOR_ABORT
 */
void signalMgt_function(int num) {
  if (num != SIGNAL_FOR_ABORT)
    error_at_line(EXIT_FAILURE, 0, __FILE__, __LINE__, "received unexpected signal %d", num);
  // Otherwise we do nothing (except discard SIGNAL_FOR_ABORT silently)
}

/**
 * @brief Thread to take care of signals
 */
void *signalMgt_thread(void *null) {
  siginfo_t info;

  int rc = pthread_detach(pthread_self());
  if (rc < 0)
    error(EXIT_FAILURE, rc, "pthread_detach");

  // We iniatialize signalMgtThreadTid
  // Glibc does not provide a wrapper for this system call; call it using syscall(2).
  signalMgtThreadTid = (pid_t) syscall (SYS_gettid);

  // Note: This thread does not need to block/mask all the
  // signals as signalMgt_init() has already done it.
  // Some signals (e.g. SIGABRT) are ignored so that if there is an illegal instruction 
  // during execution, we are able to locate where it took place with the debugger

  do{
    do {
      rc = sigwaitinfo(&mask,&info);
    } while (rc < 0 && errno == EINTR);
    if (rc < 0)
     error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "sigwaitinfo");
    // The following "if" sequence cannot be replaced by a "switch"
    // because SIGNAL_FOR_ABORT is not a constant
    if (info.si_signo == SIGALRM){
      // SIGALRM can only be delivered because we called setitimer in
      // comm.c and we came to expiration
      comm_abort_whenIT();
    } else {
      fprintf(stderr,"Signal %d received", info.si_signo);
      // As this should never happen here, we exit
      exit(EXIT_FAILURE);
    }
  } while (true);
  return NULL;
}

void signalMgt_initialize() {
  static bool done = false;
  if (!done) {
    int rc;
    struct sigaction action;

    // We block SIGALRM signals handled by the main thread so that all created threads
    // will inherit from the signal mask of the main thread.
    // Note: we do not block all signals, so that other signals (e.g. SIGABRT) 
    // are received at the level where the error occurs
    sigemptyset(&mask);
    sigaddset(&mask,SIGALRM);
    rc = pthread_sigmask(SIG_BLOCK, &mask, NULL);
    if (rc < 0)
      error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_sigmask");

    // For SIGNAL_FOR_ABORT signal, we put in place a signal handler
    action.sa_handler = signalMgt_function;
    sigemptyset(&(action.sa_mask));
    action.sa_flags = SA_INTERRUPT;
    if (sigaction(SIGNAL_FOR_ABORT, &action, NULL) != 0) 
      error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "sigaction");

    // Now we can create the thread which will take care of all signals.
    rc = pthread_create(&thread, NULL, signalMgt_thread, NULL);
    if (rc < 0)
      error(EXIT_FAILURE, rc, "pthread_create");

    done = true;
  }
}
