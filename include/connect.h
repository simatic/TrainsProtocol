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
#include "bqueue.h"

/**
 * @brief Data structure holding a trComm and a queue
 */
typedef struct{
  trComm *aComm;      /**< trComm to transmit to the treatment thread */
  trBqueue *msgQueue; /**< Queue holding the different messages to transfer to the treatment thread */
} trCommAndQueue;

/**
 * @brief Open the connection with @a addr
 * @param[in] addr The address to connect to
 * @param[in] isPred Indicates if this connection is opened towards
 *            a future predecessor
 * @return -1 if it fails and 1 if not
 */
int openConnection(address addr, bool isPred);

/**
 * @brief Close the connection with @a addr
 * @param[in] addr The address to diconnect with
 * @param[in] isPred Indicates if this connection is opened towards
 *            a future predecessor
 */
void closeConnection(address addr, bool isPred);

/**
 * @brief Search a successor in @a globalAddrArray related to @a add and create the connectcion if possible
 * @param[in] add The address to search from
 * @return The address of the successor
 * @note Return myAddress if no succ is found
 */
address searchSucc(address add);

/**
 * @brief Manage the connection : enqueue the messages received in a bqueue read by a second thread
 * @param[in] arg A pointer on an argument
 */
void *connectionMgt(void *arg);

/**
 * @brief Treat the messages received
 * @param[in] arg A pointer on a structure containing the arguments
 */
void *msgTreatment(void *arg);

#endif /* _CONNECT_H */
