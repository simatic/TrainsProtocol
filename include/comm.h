/**
 * @brief Communication module: handles all that is related to inter-machine communications
 * @file comm.h
 * @author Michel SIMATIC
 * @date  15/04/2012
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
  pthread_mutex_t mutexForSynch; /**< Mutex used to guarantee a correct synchronization between \ref comm_abort() and the long IO in progress */
  pthread_t ownerMutexForSynch;  /**< Thread owning \a mutexForSynch (if \a pthread_equal to \a pthread_null, then no owner) */
  bool aborted;                  /**< True if \ref comm_abort() was called during a long IO */
} t_comm;

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
t_comm *comm_newAndConnect(char *hostname, char *port, int connectTimeout);

/**
 * @brief Creates a new handle which will be used for accepting connections on \a port (thanks to \ref comm_accept()).
 * @param[in] port Port on which to accept connections
 * @return Pointer to the communication handle or NULL if there was an error.
 * @note If \a NULL is returned, \a errno is positionned.
 */
t_comm *comm_newForAccept(char *port);

/**
 * @brief Waits until a connection request is receive or \ref comm_abort() is called on this communication handle
 * @param[in] aComm Communication handle to work on
 * @return The new communication handle (if a connection request has been received) or \a NULL.
 * @note If \a NULL is returned, \a errno contains \a EINTR in case \ref comm_abort() has been called on this communication handle or an other value otherwise.
 */
t_comm *comm_accept(t_comm *aComm);

/**
 * @brief Aborts the I/O taking place on \a aComm
 * @param[in] aComm Communication handle to work on
 */
void comm_abort(t_comm *aComm);

/**
 * @brief Attempts to read up to \a count bytes from communication handle \a aComm into the buffer starting at \a buf.
 * @param[in] aComm Communication handle to read
 * @param[in,out] buf Buffer in which to store the read data
 * @param[in] count Maximum number of bytes to be read
 * @return Number of bytes read or -1 in case of interrupted read or error.
 * @note If -1 is returned, \a errno contains \a EINTR in case \ref comm_abort() has been called on this communication handle or an other value otherwise.
 */
int comm_read(t_comm *aComm, void *buf, size_t count);

/**
 * @brief Writes up to \a count bytes from the buffer pointed \a buf to the communication handle \a aComm
 * @param[in] aComm Communication handle to write
 * @param[in] buf Buffer in which to read the data to write
 * @param[in] count Number of bytes to write
 * @return Number of bytes written or -1 in case of interrupted write or error. 
 * @note If -1 is returned, \a errno contains \a EINTR in case \ref comm_abort() has been called on this communication handle or an other value otherwise.
 */
int comm_write(t_comm *aComm, const void *buf, size_t count);

/**
 * @brief Writes \a iovcnt buffers of data described by \a iov to the communication handle \a aComm.
 * @param[in] aComm Communication handle to write
 * @param[in] iov Array of \a iovec structures describing where the data to be written are located
 * @param[in] iovcnt Number of elements in \a iov
 * @return Number of bytes written or -1 in case of interrupted write or error.
 * @note If -1 is returned, \a errno contains \a EINTR in case \ref comm_abort() has been called on this communication handle or an other value otherwise.
 */
int comm_writev(t_comm *aComm, const struct iovec *iov, int iovcnt);


/**
 * @brief Frees @a aComm.
 * @param[in] aComm Communication handle to write
 * @note
 * <ol>
 * <li>If the communication handle corresponds to a connection, it is closed.</li>
 * <li>This procedure calls \ref comm_abort() in order to abort any long I/O done on this communication handle.</li>
 * </ol>
 */
void comm_free(t_comm *aComm);

#endif /* _COMM_H_ */
