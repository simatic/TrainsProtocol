 /**
 * @brief Functions used to connect and diconnect
 * @file connect.h
 * @author Nathan REBOUD - Damien GRAUX
 * @date 05 june 2012
 */

#ifndef _CONNECT_H
#define _CONNECT_H

#include "address.h"
#include "management_addr.h"
#include "comm.h" 

/**
 * @brief Open the connection with @a addr
 * @param[in] addr The address to connect to
 * @param[in] isPred Indicates if this connection is opened towards
 *            a future predecessor
 * @return -1 if it fails and 1 if not
 */
int open_connection(address addr, bool isPred);

/**
 * @brief Close the connection with @a addr
 * @param[in] addr The address to diconnect with
 * @param[in] isPred Indicates if this connection is opened towards
 *            a future predecessor
 */
void close_connection(address addr, bool isPred);

/**
 * @brief Search a successor in @a global_addr_array related to @a add and create the connectcion if possible
 * @param[in] add The address to search from
 * @return The address of the successor
 * @note Return my_address if no succ is found
 */
address searchSucc(address add);

/**
 * @brief Manage the connection
 * @param[in] arg A pointer on an argument
 */
void *connectionMgt(void *arg);

#endif /* _CONNECT_H */
