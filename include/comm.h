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

 Developer(s): Michel Simatic, Arthur Foltz, Damien Grau, Nicolas Hascoet, Nathan Reboud
 */

/**
 * @brief Communication module: handles all that is related to inter-machine communications
 * @file comm.h
 * @author Michel SIMATIC
 * @date  15 april 2012
 */

#ifndef _COMM_H_
#define _COMM_H_
#include <pthread.h>
#include <sys/uio.h>
#include "common.h"

/** 
 * @brief Data structure holding a communication handle
 */
typedef struct{
  int  fd;                       /**< File descriptor of the communication handle */
  pthread_mutex_t mutexForSynch; /**< Mutex used to guarantee a correct synchronization between \ref commAbort() and the long IO in progress */
  pthread_t ownerMutexForSynch;  /**< Thread owning \a mutexForSynch (if \a pthread_equal to \a pthread_null, then no owner) */
  bool aborted;                  /**< True if \ref commAbort() was called during a long IO */
} trComm;

/**
 * @brief Creates a new handle and connects it to host with \a hostname on \a port
 * @param[in] hostname Hostname of the host to connect to
 * @param[in] port Port to connect to on host \a hostname
 * @param[in] connectTimeout Timeout (in milliseconds) to use instead of system timeout  when attempting connection. If \a connectTimeout is 0, the system timeout is used: it is about 20 seconds on Linux). 
 * @return Pointer to the communication handle or NULL if there was an error.
 * @note
 * <ul>
 * <li>If \a NULL is returned, \a errno is positionned.</li>
 * <li>When there is a connect timeout, \a errno is either \a EINTR or \a ETIMEDOUT.</li>
 * </ul>
 */
trComm *commNewAndConnect(char *hostname, char *port, int connectTimeout);

/**
 * @brief Creates a new handle which will be used for accepting connections on \a port (thanks to \ref commAccept()).
 * @param[in] port Port on which to accept connections
 * @return Pointer to the communication handle or NULL if there was an error.
 * @note If \a NULL is returned, \a errno is positionned.
 */
trComm *commNewForAccept(char *port);

/**
 * @brief Waits until a connection request is receive or \ref commAbort() is called on this communication handle
 * @param[in] aComm Communication handle to work on
 * @return The new communication handle (if a connection request has been received) or \a NULL.
 * @note If \a NULL is returned, \a errno contains \a EINTR in case \ref commAbort() has been called on this communication handle or an other value otherwise.
 */
trComm *commAccept(trComm *aComm);

/**
 * @brief Aborts the I/O taking place on \a aComm
 * @param[in] aComm Communication handle to work on
 */
void commAbort(trComm *aComm);

/**
 * @brief Attempts to read up to \a count bytes from communication handle \a aComm into the buffer starting at \a buf.
 * @param[in] aComm Communication handle to read
 * @param[in,out] buf Buffer in which to store the read data
 * @param[in] count Maximum number of bytes to be read
 * @return Number of bytes read or -1 in case of interrupted read or error.
 * @note If -1 is returned, \a errno contains \a EINTR in case \ref commAbort() has been called on this communication handle or an other value otherwise.
 */
int commRead(trComm *aComm, void *buf, size_t count);

/**
 * @brief Attempts to read up to \a count bytes from communication handle \a aComm into the buffer starting at \a buf. The difference between \ref commRead() and commReadFully() is that comm_readfully() blocks until it has read \a count bytes or there is an interruption or an error. 
 * @param[in] aComm Communication handle to read
 * @param[in,out] buf Buffer in which to store the read data
 * @param[in] count Number of bytes to be read
 * @return Number of bytes read or 0 in case of interrupted read or error.
 * @note If 0 is returned, \a errno contains \a EINTR in case \ref commAbort() has been called on this communication handle or an other value otherwise.
 */
int commReadFully(trComm *aComm, void *buf, size_t count);

/**
 * @brief Writes up to \a count bytes from the buffer pointed \a buf to the communication handle \a aComm
 * @param[in] aComm Communication handle to write
 * @param[in] buf Buffer in which to read the data to write
 * @param[in] count Number of bytes to write
 * @return Number of bytes written or -1 in case of interrupted write or error. 
 * @note If -1 is returned, \a errno contains \a EINTR in case \ref commAbort() has been called on this communication handle or an other value otherwise.
 */
int commWrite(trComm *aComm, const void *buf, size_t count);

/**
 * @brief Writes \a iovcnt buffers of data described by \a iov to the communication handle \a aComm.
 * @param[in] aComm Communication handle to write
 * @param[in] iov Array of \a iovec structures describing where the data to be written are located
 * @param[in] iovcnt Number of elements in \a iov
 * @return Number of bytes written or -1 in case of interrupted write or error.
 * @note If -1 is returned, \a errno contains \a EINTR in case \ref commAbort() has been called on this communication handle or an other value otherwise.
 */
int commWritev(trComm *aComm, const struct iovec *iov, int iovcnt);


/**
 * @brief Frees @a aComm.
 * @param[in] aComm Communication handle to write
 * @note
 * <ol>
 * <li>If the communication handle corresponds to a connection, it is closed.</li>
 * <li>This procedure calls \ref commAbort() in order to abort any long I/O done on this communication handle.</li>
 * </ol>
 */
void freeComm(trComm *aComm);

#endif /* _COMM_H_ */
