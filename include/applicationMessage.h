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
 * @brief Definitions related to messages carried by trains protocol for the application using the middleware.
 * @file applicationMessage.h
 * @author Michel SIMATIC
 * @date 26 june 2012
 */

#ifndef _APPLICATION_MESSAGE_H
#define _APPLICATION_MESSAGE_H

#include "address.h"


/** 
 * @brief Type used to store message type
 */
typedef unsigned char t_typ;

/** 
 * @brief Header of messages carried by trains protocol for the application using the middleware
 */
typedef struct {
    int   len;   /**< Length of whole message */
    t_typ typ;   /**< Type of message (AM_ARRIVAL, AM_DEPARTURE; AM_TERMINATE, or a application-defined message) */
  } __attribute__((packed)) messageHeader;  

/** 
 * @brief Messages carried by trains protocol for the application using the middleware
 *
 * The @a payload contains the information carried by the protocol for the application.
 */
typedef struct message{
  messageHeader header;             /**< Header of the application message */
  char           payload[];          /**< Payload (i.e. contents) of the message */
} __attribute__((packed)) message;

/**
 * @brief Type of message corresponding to the arrival of a process
 */
#define AM_ARRIVAL 0

/**
 * @brief Type of message corresponding to the departure of a process
 */
#define AM_DEPARTURE 1

/**
 * @brief Type of message used internally by oDeliveries to exit its main loop
 */
#define AM_TERMINATE 2

/**
 * @brief First value reserved for future Application message
 */
#define AM_RESERVED_FOR_FUTURE_USE_1 3

/**
 * @brief Second value reserved for future Application message
 */
#define AM_RESERVED_FOR_FUTURE_USE_2 4

/**
 * @brief First value available to application for message typ
 */
#define FIRST_VALUE_AVAILABLE_FOR_MESS_TYP (AM_RESERVED_FOR_FUTURE_USE_2 + 1)

/** 
 * @brief Payload when arrival or departure
 */
typedef struct{
  address        ad;                 /**< Address concerned by the arrival or departure */
  address        circuit;            /**< Circuit after arrival or departure */
} __attribute__((packed)) payloadArrivalDeparture;

/**
 * @brief Macro to compute the payload of message @a mp
 */
#define payloadSize(mp) ((mp)->header.len - sizeof(messageHeader))

/** 
 * @brief View (members, who joined, who departed) of the circuit.
 *
 * Notice that if @a addrIsNull(cv_joined) (respectively @a addrIsNull(cv_departed)) is false, a new
 * process has joined (respectively left) the list of processes members of the circuit (thus participating 
 * to the trains protocol). In this case, the address found in @a cv_joined (respectively @a cv_departed) 
 * can (respectively cannot) be found in @a cv_members[]
 */
typedef struct {
  short       cv_nmemb;              /**< Number of members */
  address     cv_members[MAX_MEMB];  /**< List of members */
  address     cv_joined;             /**< New member, if any */
  address     cv_departed;           /**< Departed member, if any */
} circuitView;

/** 
 * @brief Type of function called by trains middleware when there is a change in circuit members
 */
typedef  void (*CallbackCircuitChange)(circuitView*);

/** 
 * @brief Type of function called by trains middleware when it is ready to o-deliver a message 
 * to the application layer
 */
typedef  void (*CallbackODeliver)(address,t_typ,message*);

/** 
 * @brief Callback function called by trains middleware when there is a change in circuit members
 */
extern CallbackCircuitChange theCallbackCircuitChange;

/** 
 * @brief Callback function called by trains middleware when it is ready to o-deliver a message 
 * to the application layer
 */
extern CallbackODeliver theCallbackODeliver;


/**
 * @brief Request for a pointer on a new message with a payload of size @a payloadSize
 * @param[in] payloadSize Size requested for the @a payload field of the returned message
 * @return pointer on a message upon successful completion, or NULL if an error occurred 
 * (in which case, @a trErrno is set appropriately)
 */
message *newmsg(int payloadSize);

/**
 * @brief o-broadcast of message @a mp
 * @param[in] messageTyp This parameter is a @a t_typ field greater or equal to @a FIRST_VALUE_AVAILABLE_FOR_MESS_TYP which can be used by the application  arbitrarily. The intent is that it could be used to name different kinds of data messages so they can be differentiated without looking into the body of the message.
 * @param[in] mp Message to be o-broadcasted
 * @return 0 upon successful completion, or -1 if an error occurred (in which case, @a trErrno is set appropriately)
 */
int oBroadcast(t_typ messageTyp, message *mp);

/**
 * @brief Function (to be executed by a thread) responsible for delivering messages stored in
 * global variable @a wagonsToDeliver to application layer
 * @param[in] null Unused parameter
 */
void *oDeliveries(void *null);

#endif /* _APPLICATION_MESSAGE_H */
