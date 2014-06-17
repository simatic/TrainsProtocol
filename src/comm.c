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

 
#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT  0x501

#include <memory.h> //to use memset
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "missingInMingw.h"

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/tcp.h>
#endif
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include "comm.h"
#include "counter.h"
#include "errorTrains.h"

/**
 * @brief Allocates and initializes a \a trComm structure with \a fd
 * @param[in] fd File descriptor managed by the allocated communication handle
 * @return The communication handle
 */
#ifndef WINDOWS
trComm *commAlloc(int fd){
  trComm *aComm = malloc(sizeof(trComm));
  assert(aComm != NULL);
  aComm->fd = fd;
  return aComm;
}
#else
trComm *commAlloc(SOCKET fd){
  trComm *aComm = malloc(sizeof(trComm));
  assert(aComm != NULL);
  aComm->fd = fd;
  return aComm;
}
#endif

/**
 * @brief Same as \a connect() system call, except that a \a connectTimeout can be 
   specified
 * @param[in] sockfd File descriptor of the referred socket
 * @param[in] addr Address to connect to
 * @param[in] addrlen Size of \a addr
 * @param[in] connectTimeout Timeout (in milliseconds) to use instead of system timeout  when attempting connection. If \a connectTimeout is 0, the system timeout is used: it is about 20 seconds on Linux). 
 * @return If  the  connection succeeds, zero is returned.  On error, -1 is returned, and errno is set appropriately.
 */
#ifdef WINDOWS
int connectWithTimeout (int ConnectSocket, const struct sockaddr *addr, socklen_t addrlen, int connectTimeout) {	
    int iResult;
 
    //set the socket in non-blocking
    unsigned long iMode = 1;
    iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
    if (iResult == SOCKET_ERROR)
		return -1;
	 
	iResult=connect(ConnectSocket,addr,addrlen);
    if(iResult == SOCKET_ERROR)
    {	
        int lastError = WSAGetLastError();
		if (lastError == WSAEWOULDBLOCK){
			TIMEVAL Timeout;
			Timeout.tv_sec = connectTimeout/1000;
			Timeout.tv_usec = (connectTimeout%1000)*1000;
			// restart the socket mode
			iMode = 0;
			iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
			if (iResult == SOCKET_ERROR)
				return -1;
 
			fd_set Write, Err;
			FD_ZERO(&Write);
			FD_ZERO(&Err);
			FD_SET(ConnectSocket, &Write);
			FD_SET(ConnectSocket, &Err);
 
			// check if the socket is ready
			iResult = select(0,NULL,&Write,&Err,&Timeout);		
			if(iResult == SOCKET_ERROR) {
				return -1;
			} else if (iResult == 0  || FD_ISSET(ConnectSocket, &Err)) {
				return -1;
			}
		} else {
			return -1;
		}
	}
	return 1;
}
#else
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
#endif
trComm *commNewAndConnect(char *hostname, char *port, int connectTimeout){
  
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s;
  int rc;
  int status=1;

#ifndef WINDOWS
  int fd;
#else
  WSADATA wsaData;
  SOCKET fd = INVALID_SOCKET; // is equivalent to fd in windows, 
							//the difference is that it may take any value in the range 0 to INVALID_SOCKET–1
  s = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (s != 0) {
        printf("WSAStartup failed with error: %d\n", s);
        return NULL;
    }
  
#endif

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
#ifdef WINDOWS
	WSACleanup();
#endif
  }
  
  // getaddrinfo() returns a list of address structures.
  // Try each address until we successfully connect(2).
  // If socket(2) (or connect(2)) fails, we (close the socket
  // and) try the next address. */
  
  for (rp = result; rp != NULL; rp = rp->ai_next) {

    fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
#ifndef WINDOWS
    if (fd == -1)
		continue;
#else
	if (fd == INVALID_SOCKET){
		WSACleanup();
		continue;
	}
#endif

    rc = connectWithTimeout(fd , rp->ai_addr, rp->ai_addrlen, connectTimeout);

    if (rc > 0)
      break;                  /* Success */
    
#ifndef WINDOWS
    if (close(fd) < 0)
      ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "close");
#else
	if (closesocket(fd) != 0)
      ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "closesocket");
#endif

  }

  freeaddrinfo(result);           /* No longer needed */

  if (rp == NULL) {               /* No address succeeded */
    return NULL;
  }

  // We set TCP_NODELAY flag so that packets sent on this TCP connection
  // will not be delayed by the system layer
  if (setsockopt(fd,IPPROTO_TCP, TCP_NODELAY, (char*)&status,sizeof(status)) != 0){
    //free(aComm);
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "setsockopt");
#ifdef WINDOWS
  WSACleanup();
#endif
  }

  // Everything went fine: we can return a communication handle.
#ifdef WINDOWS
  WSACleanup();
#endif
  return commAlloc(fd);
}

trComm *commNewForAccept(char *port){
  int s, on = 1;
  struct addrinfo hints;
  struct addrinfo *result, *rp;
#ifndef WINDOWS
  int fd;
#else
  WSADATA wsaData;
  SOCKET fd = INVALID_SOCKET; // is equivalent to fd in windows, 
							//the difference is that it may take any value in the range 0 to INVALID_SOCKET–1
  s = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (s != 0) {
        printf("WSAStartup failed with error: %d\n", s);
        return NULL;
    }
  
#endif
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
#ifdef WINDOWS
	WSACleanup();
#endif
    exit(EXIT_FAILURE);
  }

  /* getaddrinfo() returns a list of address structures.
     Try each address until we successfully bind(2).
     If socket(2) (or bind(2)) fails, we (close the socket
     and) try the next address. */

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    fd = socket(rp->ai_family, rp->ai_socktype,
		 rp->ai_protocol);
#ifndef WINDOWS
    if (fd == -1)
		continue;
#else
	if (fd == INVALID_SOCKET){
		WSACleanup();
		continue;
	}
#endif

    // We position the option to be able to reuse a port in case this port
    // was already used in a near past by another process
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) != 0)
      continue;

    if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0)
      break;                  /* Success */

#ifndef WINDOWS
    if (close(fd) < 0)
      ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "close");
#else
	if (closesocket(fd) != 0)
      ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "closesocket");
#endif     
  }

  if (rp == NULL) {               /* No address succeeded */
    fprintf(stderr, "%s:%d: Could not bind\n", __FILE__, __LINE__);
#ifdef WINDOWS
	WSACleanup();
#endif
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(result);           /* No longer needed */

  // We want to accept 5 simultaneous connections
  if( listen (fd,5) != 0 )
    return NULL;

  // Everything went fine: we can return a communication handle.
  return commAlloc(fd);
}

trComm *commAccept(trComm *aComm){
  int s;
  int status=1;
#ifndef WINDOWS
  int connection;
#else
  WSADATA wsaData;
  SOCKET connection = INVALID_SOCKET; // is equivalent to fd in windows, 
							//the difference is that it may take any value in the range 0 to INVALID_SOCKET–1
  s = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (s != 0) {
        printf("WSAStartup failed with error: %d\n", s);
        return NULL;
    }
#endif

  connection = accept(aComm->fd,NULL,NULL);
#ifndef WINDOWS
  if(connection < 0) 
    return NULL;
#else
  if(connection == INVALID_SOCKET){
    WSACleanup();
    return NULL;
  }
#endif

  // We set TCP_NODELAY flag so that packets sent on this TCP connection
  // will not be delayed by the system layer
  if (setsockopt(connection,IPPROTO_TCP, TCP_NODELAY, (char*)&status, sizeof(status)) != 0){
#ifdef WINDOWS
    WSACleanup();
#endif
    return NULL;
  }
  // Everything went fine: we can return a communication handle.
#ifdef WINDOWS
  WSACleanup();
#endif
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

 
#ifndef WINDOWS    
  if (shutdown(aComm->fd, SHUT_RDWR) != 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "shutdown"); 
  if (close(aComm->fd) != 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "close");
#else
  if (shutdown(aComm->fd, SD_BOTH) != 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "shutdown"); 
  if (closesocket(aComm->fd) != 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "closesocket");

#endif

  free(aComm);
}

