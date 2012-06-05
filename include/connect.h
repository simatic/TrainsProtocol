/**
 * @brief Fun used to connect
 * @file connect.h
 * @author Nathan REBOUD Damien GRAUX
 * @date 05/06/2012
 */

#ifndef _CONNECT_H
#define _CONNECT_H

#include "address.h"
#include "management_addr.h"
#include "comm.h" 

/**
 * @brief Open the connection
 * @param[in] addr The address to connect to
 * @return -1 if it fails and 1 if not
 */
int open_connection(address addr);

/**
 * @brief Close the connection
 * @param[in] addr The address to diconnect
 */
void close_connection(address addr);

/**
 * @brief Search a successor in @a global_addr_array related to @a add and create the connectcion if possible
 * @param[in] add The address to search from
 * @return The address of the successor
 * @note Return my_address if no succ is found
 */
address searchSucc(address add);

#endif /* _CONNECT_H */
