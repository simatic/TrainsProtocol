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
 * @brief Definitions of the complex structures used to manage properly the pointers
 * @file advanced_struct.h
 * @author Damien GRAUX
 * @date 05 june 2012
 */

#ifndef _ADVANCED_STRUCT_H
#define _ADVANCED_STRUCT_H

#include <pthread.h>
#include "msg.h"

/**
 * @brief Descritpion of the structure which will be placed before the messages
 * @note Holds two parameters : a @a mutex to control the writting and a @a counter
 */
typedef struct{
  pthread_mutex_t mutex;/**<To make the writting and the reading of the train safe*/
  int counter;/**<Count the number of pointers refering to the wagons of the train to know when erase the train*/
}prefix;

/**
 * @brief Data structure used to improve the messages (type Msg) and wagonToSend (type wagon) with a prefixe
 * @note Stand for "Wagon Or Message In Memory"
 * @note Used only for Messages and WagonToSend. In fact, when wagons are part of a train they do not bring a @a womim struc
ture, they are part of a bigger structure called Msg.
 */
typedef struct{
  prefix pfx;
  union{/**<A union between to two possible types*/
    wagon wagon;
    Msg msg;
  };
}womim;

/**
 * @brief Structure to watch the behaviour or wagon_to_send and messages
 * @note wiw stands for : A wagon in a womim
 * @note It links wagon_to_send and messages and the prefixe head-on the train
 */
typedef struct {
  wagon* p_wagon;
  womim* p_womim;
}wiw;

/**
 * @brief Data structure for lts
 * @note lts is for : Last Train Send
 */
typedef struct {
  int   lng;/**<the length of the structure*/
  MType type;/**<the type of message used*/
  stamp stamp;/**<the stamp which contains some info*/
  addressSet circuit;/**<a log of the circuit*/
  struct {
    wiw w_w;
    int len;
  } w;/**<The area used to stock wagons*/
  wiw* p_wtosend;/**<refers to the wagon which is bouned to be sent*/
}ltsStruct;

/**
 * @brief Definition of an array of ltsStruct
 * @note @a MAX_NTR refers to maximal number of trains possible -> Define in constant.h
 */
typedef ltsStruct ltsArray[MAX_NTR];

/**
 * @brief Go to the next wagon
 * @param[in] msg_ext A pointer on an womim whom union represents a Msg
 * @param[in] w The wagon whom we need the next
 * @return A pointer on the next wagon
 * @note The result NULL is rised if the end is reached
 */
wagon* nextWagon(womim* msg_ext, wagon* w);

/**
 * @brief Tests if ad is member of all the circuit of lts.
 * @param[in] ad The address to test
 * @param[in] ltsarray The lts of the protocol
 * @return A boolean
 */
bool isInLts(address  ad, ltsArray ltsarray);

/**
 * @brief Says if the train signalized by @a tr_st is recent or no
 * @param[in] tr_st The stamp of the train
 * @param[in] lts Array of last trains sent
 * @param[in] last_id The last id sent
 * @return A boolean
 * @note This fun holds the overflows
 */
bool isRecentTrain(stamp tr_st,ltsArray lts, char last_id);

/**
 * @brief Create a new wiw
 * @return A pointer on a wiw
 */
wiw * newWiw();

/**
 * @brief Allocate a message in @a wagonToSend
 * @param[in] payloadSize An int
 * @return A pointer on a message
 */
message * mallocWiw(int payloadSize);

/**
 * @brief Look after the counter and decrements it (does not free the wiw)
 * @param[in] ww A pointer on a wiw used to have prefixe and the rest.
 */
void releaseWiw(wiw * ww);

/**
 * @brief Look after the counter and free the wiw if it is equal to 0
 * @param[in] ww A pointer on a wiw used to have prefixe and the rest.
 */
void freeWiw(wiw * ww);

/**
 * @brief Decreases counter of womim @a wo and frees it if it is 0.
 * @param[in] wo A pointer to the womim
 */
void freeWomim(womim *wo);

/**
 * @brief Wiw containing the next messages to be o-broadcasted
 */
extern wiw * wagonToSend;

/**
 * @brief Mutex protecting access to wagonToSend
 */
extern pthread_mutex_t mutexWagonToSend;

/**
 * @brief Condition about wagonToSend
 */
extern pthread_cond_t condWagonToSend;


#endif /* _ADVANCED_STRUCT_H */
