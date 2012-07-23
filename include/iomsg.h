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
 * @brief This file gives function to send and receive Msg
 * @file iomsg.h
 * @author Damien GRAUX
 * @date 30 may 2012
 */

#ifndef _IOMSG_H
#define _IOMSG_H

#include "address.h"
#include "management_addr.h"
#include "comm.h"
#include "msg.h"
#include "advanced_struct.h"
#include "connect.h"

/**
 * @brief Fun used to listen on a t_comm
 * @param[in] aComm id of the connection to listen
 * @return A pointer of womim
 * @note If an error occured, returns a Msg with the MType DEFAULT
 * @warning Do not forget to free after the Msg returned
 */
womim * receive(t_comm * aComm);

/**
 * @brief Function used to send @a msg to @a addr
 * @param[in] addr The address where the @a msg will be sent
 * @param[in] isPred Indicates if this connection is opened towards
 *            a future predecessor
 * @param[in] type The MType of the message which is bouned to be sent
 * @param[in] sender The address of the sender
 * @return The number of bites sent
 * @note If the number of bites sent is diferent of the size of the msg, the sending is automatically done again (... and again)
 * @note If the @a type is TRAIN an error will be given
 * @note It is -1 if it fails
 * @note It uses the variable "globalAddrArray" which is created in management_addr.h
 */
int sendOther(address addr, bool isPred, MType type, address sender);

/**
 * @brief Function specialized for sending train with @a lts
 * @note It's cool because it avoids the creatioin of a heavy train structure... ^^
 * @param[in] addr The address where the train will be sent
 * @param[in] isPred Indicates if this connection is opened towards
 *            a future predecessor
 * @param[in] lts The lst_struct used to built the train
 * @return The number of bites sent
 * @note If the number of bites sent is diferent of the size of the msg, the sending is automatically done again (... and again)
 * @note It is -1 if it fails
 * @note It uses the variable "globalAddrArray" which is created in management_addr.h
 */
int sendTrain(address addr, bool isPred, ltsStruct lts);

#endif /* _IOMSG_H */
