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

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <stdio.h>

#include "comm.h"
#include "counter.h"
#include "errorTrains.h"

/**
 * @brief Allocates and initializes a \a trComm structure with \a fd
 * @param[in] fd File descriptor managed by the allocated communication handle
 * @return The communication handle
 */
trComm *commAlloc(int fd){
  trComm *aComm = malloc(sizeof(trComm));
  assert(aComm != NULL);
  aComm->fd = fd;
  return aComm;
}

/**
 * @brief Same as \a connect() system call, except that a \a connectTimeout can be 
   specified
 * @param[in] sockfd File descriptor of the referred socket
 * @param[in] addr Address to connect to
 * @param[in] addrlen Size of \a addr
 * @param[in] connectTimeout Timeout (in milliseconds) to use instead of system timeout  when attempting connection. If \a connectTimeout is 0, the system timeout is used: it is about 20 seconds on Linux). 
 * @return If  the  connection succeeds, zero is returned.  On error, -1 is returned, and errno is set appropriately.
 */
int connectWithTimeout(int sockfd, const struct sockaddr *addr, socklen_t addrlen, int connectTimeout) {
  long arg;
  fd_set myset; 
  struct timeval tv; 
  struct timeval *ptv; 
  int rc, valopt; 
  socklen_t lon; 

  //
  // The following code is an adaptation from the code found in 
  // http://developerweb.net/viewtopic.php?id=3196 (http://www.codeproject.com/Tips/168704/How-to-set-a-socket-connection-timeout for Windows)
  //

  // Set non-blocking 
  if( (arg = fcntl(sockfd, F_GETFL, NULL)) < 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__,  "fcntl(..., F_GETFL)"); 
  arg |= O_NONBLOCK; 
  if( fcntl(sockfd, F_SETFL, arg) < 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__,  "fcntl(..., F_SETFL)"); 
  // Trying to connect with timeout 
  rc = connect(sockfd ,addr, addrlen);
  if (rc < 0) { 
    if (errno == EINPROGRESS) { 
      // EINPROGRESS in connect() - selecting 
      if (connectTimeout != 0) {
	tv.tv_sec = connectTimeout/1000; 
	tv.tv_usec = (connectTimeout%1000)*1000;
	ptv = &tv;
      } else {
	ptv = NULL;
      }
      FD_ZERO(&myset); 
      FD_SET(sockfd, &myset); 
      rc = select(sockfd+1, NULL, &myset, NULL, ptv); 
      if (rc < 0)
	ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__,  "Error connecting"); 
      if (rc == 0) {
	// select stopped because of timeout
	errno = ETIMEDOUT;
	return -1;
      }
      // rc > 0

      // Get the return value of the connect system call
      lon = sizeof(int); 
      if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0)
	ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__,  "getsockopt");
	// Check if an error was returned
	if (valopt) {
	  // It is the case
	  errno = valopt;
	  return -1;
	}
      // Set to blocking mode again... 
      if( (arg = fcntl(sockfd, F_GETFL, NULL)) < 0)
	ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__,  "fcntl(..., F_GETFL)"); 
      arg &= (~O_NONBLOCK);
      if( fcntl(sockfd, F_SETFL, arg) < 0)
	ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__,  "fcntl(..., F_SETFL)"); 
    } else if (errno == ENETUNREACH) {
      // The desired host/port cannot be reached (because the host is down or
      // no process on this host is listening on this port)
      return -1;
    } else 
      ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__,  "Error connecting");
  }
  return 1;
}

trComm *commNewAndConnect(char *hostname, char *port, int connectTimeout){
  int fd;
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s;
  int rc;
  int status=1;

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
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, 
		  "getaddrinfo on hostname \"%s\": %s\n", hostname, gai_strerror(s));
  }
  
  // getaddrinfo() returns a list of address structures.
  // Try each address until we successfully connect(2).
  // If socket(2) (or connect(2)) fails, we (close the socket
  // and) try the next address. */
  
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (fd == -1)
      continue;

    rc = connectWithTimeout(fd , rp->ai_addr, rp->ai_addrlen, connectTimeout);

    if (rc > 0)
      break;                  /* Success */
    
    if (close(fd) < 0)
      ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "close");
  }

  freeaddrinfo(result);           /* No longer needed */

  if (rp == NULL) {               /* No address succeeded */
    return NULL;
  }

  // We set TCP_NODELAY flag so that packets sent on this TCP connection
  // will not be delayed by the system layer
  if (setsockopt(fd,IPPROTO_TCP, TCP_NODELAY, &status,sizeof(status)) < 0){
    //free(aComm);
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "setsockopt");
  }

  // Everything went fine: we can return a communication handle.
  return commAlloc(fd);
}

trComm *commNewForAccept(char *port){
  int fd, s, on = 1;
  struct addrinfo hints;
  struct addrinfo *result, *rp;

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
      ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "close");      
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
  return commAlloc(fd);
}

trComm *commAccept(trComm *aComm){
  int connection;
  int status=1;

  connection = accept(aComm->fd,NULL,NULL);

  if(connection < 0) 
    return NULL;

  // We set TCP_NODELAY flag so that packets sent on this TCP connection
  // will not be delayed by the system layer
  if (setsockopt(connection,IPPROTO_TCP, TCP_NODELAY, &status, sizeof(status)) < 0)
    return NULL;

  // Everything went fine: we can return a communication handle.
  return commAlloc(connection);
}

int commRead(trComm *aComm, void *buf, size_t count){
  int nb = recv(aComm->fd, buf, count, 0);

  if (nb > 0) {
    counters.comm_read++;
    counters.comm_read_bytes += nb;
  }

  return nb;
}

int commReadFully(trComm *aComm, void *buf, size_t count){
  int nb;
  int nbTotal = 0;

  do {
    nb = commRead(aComm, (char*)buf + nbTotal, count - nbTotal);
    if (nb < 0)
      break;
    nbTotal += nb;
  } while ((nb > 0) && (nbTotal < count));

  if (nbTotal > 0) {
    counters.comm_readFully++;
    counters.comm_readFully_bytes += nbTotal;
  }

  return nbTotal;
}

int commWrite(trComm *aComm, const void *buf, size_t count){
  int nb = send(aComm->fd, buf, count, 0);

  if (nb > 0) {
    counters.comm_write++;
    counters.comm_write_bytes += nb;
  }

  return nb;
}

int commWritev(trComm *aComm, const struct iovec *iov, int iovcnt){
  int nb = writev(aComm->fd, iov, iovcnt);

  if (nb > 0) {
    counters.comm_writev++;
    counters.comm_writev_bytes += nb;
  }

  return nb;
}

void freeComm(trComm *aComm){

  if (shutdown(aComm->fd, SHUT_RDWR) < 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "shutdown");      
  if (close(aComm->fd) < 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "close");

  free(aComm);
}

