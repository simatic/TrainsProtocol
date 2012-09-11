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
* @brief File holding the types' definition for the in/out flux
* @file msg.h
* @author Nathan REBOUD - Damien GRAUX
* @date 30 may 2012
*/
 
#ifndef _MSG_H
#define _MSG_H
 
#include <pthread.h>
#include <assert.h>

#include "address.h"
#include "wagon.h"
#include "common.h"
#include "trains.h"


/**
* @brief Data structure containing logicial clocks
*/
typedef unsigned char t_lc;

/**
 * @brief Number of possible values for t_lc
 * @note Because we can have only 256 values in an unsigned char
 */
#define M 256

/**
 * @brief Data structure for stamps
 */
typedef struct {
  char id;/**<The identifier*/
  t_lc lc;/**<The logical clock*/
  char round;/**<The roundat that moment*/
}__attribute__((packed)) stamp;

/**
* @brief Data structure for trains
*/
typedef struct {
  stamp stamp;/**<A stramp for some info*/
  addressSet circuit;/**<A description of the circuit*/
  wagon wagons[];/**<The block of wagons containing info*/
}__attribute__((packed)) Train;

/**
* @brief Data structure for Default messages
* @note Use only to init message and to coerce the right type in case of bad use of some functions
*/
typedef struct {
  int problem_id; /**<To refer the problem. Actually the values can be 0 and 1.*/
}Default;

/**
* @brief Data structure for Insert incoms
*/
typedef struct {
  address sender; /**<The reference to the sender*/
}Insert;

/**
* @brief Data structure for AckInsert incoms
*/
typedef struct {
  address sender;/**<The reference to the sender*/
}AckInsert;

/**
* @brief Data structure for NakInsert incoms
*/
typedef struct {
  address sender;/**<The reference to the sender*/
}NakInsert;

/**
* @brief Data structure for NewSucc incoms
*/
typedef struct {
  address sender;/**<The reference to the sender*/
}NewSucc;

/**
* @brief Data structure for Disconnect incoms
*/
typedef struct {
  address sender;/**<The reference to the sender>*/
}Disconnect;

/**
* @brief Data structure for MType (not defined as an enum in order to use only
* one byte instead of four
*/
typedef char MType;

#define DEFAULT         0 /**<Use to init message or to raise error*/
#define TRAIN           1
#define INSERT          2
#define ACK_INSERT      3
#define NAK_INSERT      4
#define DISCONNECT_PRED 5 /**<Disconnection with predecessor*/
#define DISCONNECT_SUCC 6 /**<Disconnection with successor*/
#define NEWSUCC         7

/**
* @brief Data structure for Msg
*/
typedef struct {
  
  int len;/**<Total length of the msg*/
  MType type;/**<MType of the message*/

  union {
    Default def;
    Train train;
    Insert insert;
    AckInsert ackInsert;
    NakInsert nakInsert;
    NewSucc newSucc;
    Disconnect disconnect;
  }body;/**<A union between all the types of possible messages*/
}__attribute__((packed)) Msg;

/**
* @brief Gives the first wagon of @a msg
* @param[in] msg A pointer on message
* @return A pointer on first wagon contained in message of train type
* @note If no wagon returns NULL
*/
wagon* firstWagon(Msg * msg);

/**
* @brief Initiate an empty Msg with DEFAULT MType
* @return An empty Msg
* @note The problem_id is 0
*/
Msg initMsg();

/**
* @brief Create a Msg of the given @a mtype
* @note If @a mtype is TRAIN then it will give a DEFAULT Msg with the problem_id number 1. This function shouldn't be used to create TRAIN.
* @param[in] mtype The MType wanted
* @param[in] addrID The address (short)of the process which is bounded to send the Msg
* @return The freshly created Msg
* @warning
* <ul><li> If an error occured during the creation of the message, the result will be a Msg with a Default body where the problem_id will be -1. In addition, an error will be rised thanks to perror.</li></ul>
*/
Msg newMsg(MType mtype, address addrID);

#endif /* _MSG_H */
