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


/**
 * @brief Fun used to listen the @a addr
 * @param[in] addr The address to listen
 * @return A pointer of womim
 * @note If an error occured, returns a Msg with the MType DEFAULT
 * @warning Do not forget to free after the Msg returned
 */
womim * receive(address addr);

/**
 * @brief Function used to send @a msg to @a addr
 * @param[in] addr The address where the @a msg will be sent
 * @param[in] msg A pointer on the Msg
 * @return The integer refering to the succeed of the sent
 * @note It is -1 if it fails
 * @note It uses the variable "global_addr_array" which is created in management_addr.h
 */
int send_other(address addr, Msg * msg);

/**
 * @brief Function specialized for sending train with @a lts
 * @note It's cool because it avoids the creatioin of a heavy train structure... ^^
 * @param[in] addr The address where the train will be sent
 * @param[in] lts The lst_struct used to built the train
 * @return The integer refering to the succeed of the sent
 * @note It is -1 if it fails
 * @note It uses the variable "global_addr_array" which is created in management_addr.h
 */
int send_train(address addr, lts_struct lts);

#endif /* _IOMSG_H */
