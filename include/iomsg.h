/**
 * @brief This file gives fun to send and receive Msg
 * @file iomsg.h
 * @author Damien GRAUX
 * @date 30/05/2012
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
 * @param[in] type The MType of the message which is bouned to be sent
 * @param[in] sender The address of the sender
 * @return The number of bites sent
 * @note If the number of bites sent is diferent of the size of the msg, the sending is automatically done again (... and again)
 * @note If the @a type is TRAIN an error will be given
 * @note It is -1 if it fails
 * @note It uses the variable "global_addr_array" which is created in management_addr.h
 */
int send_other(address addr, MType type, address sender);

/**
 * @brief Function specialized for sending train with @a lts
 * @note It's cool because it avoids the creatioin of a heavy train structure... ^^
 * @param[in] addr The address where the train will be sent
 * @param[in] lts The lst_struct used to built the train
 * @return The number of bites sent
 * @note If the number of bites sent is diferent of the size of the msg, the sending is automatically done again (... and again)
 * @note It is -1 if it fails
 * @note It uses the variable "global_addr_array" which is created in management_addr.h
 */
int send_train(address addr, lts_struct lts);

#endif /* _IOMSG_H */
