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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "signalMgt.h"
#include "common.h"
#include "errorTrains.h"

pid_t signalMgtThreadTid = 0;

static sigset_t mask;
static pthread_t thread;

void commAbortWhenIT();

/**
 * @brief Signal handler for SIGNAL_FOR_ABORT
 */
void signalMgtFunction(int num) {
  if (num != SIGNAL_FOR_ABORT)
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "received unexpected signal %d", num);
  // Otherwise we do nothing (except discard SIGNAL_FOR_ABORT silently)
}

/**
 * @brief Thread to take care of signals
 */
void *signalMgtThread(void *null) {
  int sig;

  int rc = pthread_detach(pthread_self());
  if (rc < 0)
    ERROR(EXIT_FAILURE, rc, "pthread_detach");

  // We iniatialize signalMgtThreadTid
  // Glibc does not provide a wrapper for this system call; call it using syscall(2).
  signalMgtThreadTid = (pid_t) syscall (SYS_gettid);

  // Note: This thread does not need to block/mask all the
  // signals as signalMgt_init() has already done it.
  // Some signals (e.g. SIGABRT) are ignored so that if there is an illegal instruction 
  // during execution, we are able to locate where it took place with the debugger

  do{
    do {
      rc = sigwait(&mask,&sig);
    } while (rc < 0 && errno == EINTR);
    if (rc < 0)
     ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "sigwaitinfo");
    // The following "if" sequence cannot be replaced by a "switch"
    // because SIGNAL_FOR_ABORT is not a constant
    if (sig == SIGALRM){
      // SIGALRM can only be delivered because we called setitimer in
      // comm.c and we came to expiration
      commAbortWhenIT();
    } else {
      fprintf(stderr,"Signal %d received", sig);
      // As this should never happen here, we exit
      exit(EXIT_FAILURE);
    }
  } while (true);
  return NULL;
}

void signalMgtInitialize() {
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
      ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_sigmask");

    // For SIGNAL_FOR_ABORT signal, we put in place a signal handler
    action.sa_handler = signalMgtFunction;
    sigemptyset(&(action.sa_mask));
#ifdef SA_INTERRUPT // This #ifdef seems required by systems like MacOS
    action.sa_flags = SA_INTERRUPT;
#endif
    if (sigaction(SIGNAL_FOR_ABORT, &action, NULL) != 0) 
      ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "sigaction");

    // Now we can create the thread which will take care of all signals.
    rc = pthread_create(&thread, NULL, signalMgtThread, NULL);
    if (rc < 0)
      ERROR(EXIT_FAILURE, rc, "pthread_create");

    done = true;
  }
}
