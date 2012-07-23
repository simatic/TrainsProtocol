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

#include <error.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <stdio.h>

#include "comm.h"
#include "signalMgt.h"
#include "counter.h"

/**
 * @brief Variable to hold a null thread identifier
 */
static pthread_t pthread_null;

/**
 * @brief Variable containing the communication handle which is currently
 * doing a \a connect() system call
 */
static t_comm *commDoingConnect = NULL;

/**
 * @brief Initializes communication module
 */
void comm_initialize(){
  static bool done;
  if (!done){
    memset(&pthread_null,0,sizeof(pthread_null));
    signalMgt_initialize();
    done = true;
  }
}

/**
 * @brief Allocates and initializes a \a t_comm structure with \a fd
 * @param[in] fd File descriptor managed by the allocated communication handle
 * @return The communication handle
 */
t_comm *comm_alloc(int fd){
  t_comm *aComm = malloc(sizeof(t_comm));
  assert(aComm != NULL);
  aComm->fd = fd;
  pthread_mutex_init(&(aComm->mutexForSynch),NULL);
  aComm->ownerMutexForSynch = pthread_null;
  aComm->aborted = false;
  return aComm;
}

/**
 * @brief Prepares the communication module to do a long IO (read, write, accept, connect).
 * @param[in] aComm Communication handle to work on
 */
void comm_longIOBegin(t_comm *aComm){
  // We lock mutexForSynch, so that if there is a comm_abort() on this long
  // IO, the comm_abort() will wait until we are indeed done with  the IO
  aComm->ownerMutexForSynch = pthread_self();

  MUTEX_LOCK(aComm->mutexForSynch);
}

/**
 * @brief Notifies the communication module that a long IO (read, write, accept, connect) is done.
 * @param[in] aComm Communication handle to work on
 */
void comm_longIOEnd(t_comm *aComm){
  aComm->ownerMutexForSynch = pthread_null;
  aComm->aborted = false;

  // We release mutexForSynch, so that if an abort is waiting for
  // us to be done, it may proceed.
  MUTEX_UNLOCK(aComm->mutexForSynch);
}

t_comm *comm_newAndConnect(char *hostname, char *port, int connectTimeout){
  int fd;
  t_comm *aComm;
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s;
  int rc;
  int status=1;

  comm_initialize();

  aComm = comm_alloc(-1);

  //
  // The following code is an adaptation from the example in man getaddrinfo
  //

  // Obtain address(es) matching host/port
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* Stream socket */
  hints.ai_flags = 0;
  hints.ai_protocol = 0;          /* Any protocol */

  s = getaddrinfo(hostname, port, &hints, &result);
  if (s != 0) {
    fprintf(stderr, "%s:%d: getaddrinfo: %s\n", __FILE__, __LINE__, gai_strerror(s));
    exit(EXIT_FAILURE);
  }
  
  // getaddrinfo() returns a list of address structures.
  // Try each address until we successfully connect(2).
  // If socket(2) (or connect(2)) fails, we (close the socket
  // and) try the next address. */
  
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    fd = socket(rp->ai_family, rp->ai_socktype,
		 rp->ai_protocol);
    if (fd == -1)
      continue;
    aComm->fd = fd;
    
    comm_longIOBegin(aComm);

    if (connectTimeout != 0){
      // A connectTimeout was specified to avoid being blocked waiting for
      // the standard timeout
      struct itimerval aTimer;

      // We memorize the communication handle which will do the connect
      if (commDoingConnect == NULL)
	commDoingConnect = aComm;
      else{
	fprintf(stderr, "%s:%d: Comm module does not know to kandle simultaneous calls to comm_newAndConnect\n", __FILE__, __LINE__);
	exit(EXIT_FAILURE);
      }

      // We launch the timer
      // Upon expiration, this will deliver SIGALRM which, as defined in
      // signalMgt.c, calls comm_timeout().
      aTimer.it_value.tv_sec = connectTimeout/1000;
      aTimer.it_value.tv_usec = (connectTimeout%1000)*1000;
      aTimer.it_interval.tv_sec = 0;
      aTimer.it_interval.tv_usec = 0;
      if (setitimer(ITIMER_REAL, &aTimer, NULL) < 0)
	error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "setitimer");
    }

    do {
      rc = connect(fd , rp->ai_addr, rp->ai_addrlen);
    } while ((rc < 0) && (errno == EINTR) && !aComm->aborted);

    if (connectTimeout != 0){
      struct itimerval aTimer = {{0,0},{0,0}};
      // We stop the timer
      if (setitimer(ITIMER_REAL, &aTimer, NULL) < 0)
	error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "setitimer");

      // There is no more communication handle blocked on connect()
      commDoingConnect = NULL;
    }

    comm_longIOEnd(aComm);

    if (rc != -1)
      break;                  /* Success */
    
    if (close(fd) < 0)
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "close");
  }

  freeaddrinfo(result);           /* No longer needed */

  if (rp == NULL) {               /* No address succeeded */
    free(aComm);
    return NULL;
  }

  // We set TCP_NODELAY flag so that packets sent on this TCP connection
  // will not be delayed by the system layer
  if (setsockopt(fd,IPPROTO_TCP, TCP_NODELAY, &status,sizeof(status)) < 0){
    free(aComm);
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "setsockopt");
  }

  // Everything went fine: we can return a communication handle.
  return aComm;
}

t_comm *comm_newForAccept(char *port){
  int fd, s, on = 1;
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  comm_initialize();

  //
  // The following code is an adaptation from the example in man getaddrinfo
  //

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM;/* Stream socket */
  hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
  hints.ai_protocol = 0;          /* Any protocol */
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  s = getaddrinfo(NULL, port, &hints, &result);
  if (s != 0) {
    fprintf(stderr, "%s:%d: getaddrinfo: %s\n", __FILE__, __LINE__, gai_strerror(s));
    exit(EXIT_FAILURE);
  }

  /* getaddrinfo() returns a list of address structures.
     Try each address until we successfully bind(2).
     If socket(2) (or bind(2)) fails, we (close the socket
     and) try the next address. */

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    fd = socket(rp->ai_family, rp->ai_socktype,
		 rp->ai_protocol);
    if (fd == -1)
      continue;

    // We position the option to be able to reuse a port in case this port
    // was already used in a near past by another process
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
      continue;

    if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0)
      break;                  /* Success */

    if (close(fd) < 0)
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "close");      
  }

  if (rp == NULL) {               /* No address succeeded */
    fprintf(stderr, "%s:%d: Could not bind\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(result);           /* No longer needed */

  // We want to accept 5 simultaneous connections
  if( listen (fd,5) < 0 )
    return NULL;

  // Everything went fine: we can return a communication handle.
  return comm_alloc(fd);
}

t_comm *comm_accept(t_comm *aComm){
  int connection;
  int status=1;

  comm_longIOBegin(aComm);

  do {
    connection = accept(aComm->fd,NULL,NULL);
  } while ((connection < 0) && (errno == EINTR) && !aComm->aborted);

  comm_longIOEnd(aComm);

  if(connection < 0) 
    return NULL;

  // We set TCP_NODELAY flag so that packets sent on this TCP connection
  // will not be delayed by the system layer
  if (setsockopt(connection,IPPROTO_TCP, TCP_NODELAY, &status, sizeof(status)) < 0)
    return NULL;

  // Everything went fine: we can return a communication handle.
  return comm_alloc(connection);
}

/**
 * @brief Aborts the accept taking place on \a commDoingConnect
 * @note Procedure called when SIGALRM is delivered
 */
void comm_abort_whenIT(){
  if (commDoingConnect) {
    commDoingConnect->aborted = true;

    if (!pthread_equal(commDoingConnect->ownerMutexForSynch,pthread_null)) {
      // We send a signal to that thread so that we interrupt the slow system 
      // call (read, write, connect, accept) it is making
      pthread_kill(commDoingConnect->ownerMutexForSynch, SIGNAL_FOR_ABORT);
    }
    
    // We are under an IT ==> we must not call comm_longIOBegin() as
    // we do in com_abort(). Otherwise we will have a deadlock.
  }
}

void comm_abort(t_comm *aComm){
  aComm->aborted = true;

  if (!pthread_equal(aComm->ownerMutexForSynch,pthread_null)) {
    // We send a signal to that thread so that we interrupt the slow system 
    // call (read, write, connect, accept) it is making
    pthread_kill(aComm->ownerMutexForSynch, SIGNAL_FOR_ABORT);
  
    // We lock the mutex to wait until the slow system call is indeed over
    // and then we unlock the mutex
    comm_longIOBegin(aComm);
    comm_longIOEnd(aComm);
  }
}

int comm_read(t_comm *aComm, void *buf, size_t count){
  int nb;

  if (aComm->aborted){
    aComm->aborted = false;
    errno = EINTR;
    return -1;
  }

  comm_longIOBegin(aComm);

  do {
    nb = read(aComm->fd, buf, count);
  } while ((nb < 0) && (errno == EINTR) && !aComm->aborted);

  comm_longIOEnd(aComm);

  counters.comm_read++;
  counters.comm_read_bytes += nb;

  return nb;
}

int comm_readFully(t_comm *aComm, void *buf, size_t count){
  int nb;
  int nbTotal = 0;

  if (aComm->aborted){
    aComm->aborted = false;
    errno = EINTR;
    return -1;
  }
  do {
    nb = comm_read(aComm, (char*)buf + nbTotal, count - nbTotal);
    if (nb < 0)
      break;
    nbTotal += nb;
  } while ((nb > 0) && (nbTotal < count) && !aComm->aborted);

  counters.comm_readFully++;
  counters.comm_readFully_bytes += nbTotal;

  return nbTotal;
}

int comm_write(t_comm *aComm, const void *buf, size_t count){
  int nb;

  if (aComm->aborted){
    aComm->aborted = false;
    errno = EINTR;
    return -1;
  }

  comm_longIOBegin(aComm);

  do {
    nb = write(aComm->fd, buf, count);
  } while ((nb < 0) && (errno == EINTR) && !aComm->aborted);

  comm_longIOEnd(aComm);

  counters.comm_write++;
  counters.comm_write_bytes += nb;

  return nb;
}

int comm_writev(t_comm *aComm, const struct iovec *iov, int iovcnt){
  int nb;

  if (aComm->aborted){
    aComm->aborted = false;
    errno = EINTR;
    return -1;
  }

  // FIXME: We must comment the comm_longIOBegin(aComm);
  // otherwise it is not possible to do a read and a write at the
  // same time on the socket
  //comm_longIOBegin(aComm);

  do {
    nb = writev(aComm->fd, iov, iovcnt);
  } while ((nb < 0) && (errno == EINTR) && !aComm->aborted);

  // FIXME : comm_longIOEnd(aComm); is commented because
  // comm_longIOBegin(aComm); hereabove is commented
  //comm_longIOEnd(aComm);

  counters.comm_writev++;
  counters.comm_writev_bytes += nb;

  return nb;
}

void comm_free(t_comm *aComm){
  int rc;
  comm_abort(aComm);
  if (close(aComm->fd) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "close");
  rc = pthread_mutex_destroy(&(aComm->mutexForSynch));
  if(rc)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_mutex_destroy");
  free(aComm);
}

