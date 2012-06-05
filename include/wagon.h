/**
 * @brief Definitions related to wagons used in trains protocol.
 */

#ifndef _wagon_H
#define _wagon_H

#include "address.h"
#include "applicationMessage.h"
#include "bqueue.h"

/** 
 * @brief Maximum length of a wagon containing several messages. NB: if an application message
 * is bigger than WAGON_MAX_LEN, it is accepted by the communication middleware, but must
 * be stored alone in the wagon.
 */
#define WAGON_MAX_LEN (1<<15)

/** 
 * @brief Header of wagons carried by trains protocol
 */
typedef struct{
    int            len;                /**< Length of whole wagon */
    address        sender;             /**< Sender of wagon */
    char           round;              /**< Round to which this wagon belongs */
} __attribute__((packed)) wagon_header;

/** 
 * @brief Wagon carried by trains protocol
 */
typedef struct wagon{
  wagon_header   header;             /**< header of wagon */
  message        msgs[];             /**< Messages carried by this wagon */
} __attribute__((packed)) wagon;

/** 
 * @brief Wagon containing the next messages to be uto-broadcasted
 */
extern wagon *wagonToSend_outdated;

/** 
 * @brief Mutex protecting access to wagonToSend_outdated
 */
extern pthread_mutex_t mutexWagonToSend_outdated;

/** 
 * @brief Condition protecting access to wagonToSend_outdated
 */
extern pthread_cond_t condWagonToSend_outdated;

/** 
 * @brief Bqueue containing the wagons (received from trains protocol) which can be delivered
 * to the application layer
 */
extern t_bqueue *wagonsToDeliver;

/**
 * @brief Returns an empty wagon (its header.sender field is initialized to my_address
 * @return pointer to an empty wagon
 * @warning OUTDATED -- @a newwiw is better !!
 */
wagon *newwagon_outdated();

/**
 * @brief Returns pointer on first message contained in wagon @a w
 * @param[in] w Wagon in which to search first message
 * @return pointer on a first message or NULL if @a w contains no messages
 */
message *firstmsg(wagon *w);

/**
 * @brief Returns pointer on message following message @a mp in wagon @a w
 * @param[in] w Wagon in which to search next message
 * @param[in] mp Message after which to search for an other message
 * @return pointer on a next message or NULL if @a w contains no other messages
 */
message *nextmsg(wagon *w, message *mp);

/**
 * @brief In wagon pointed by @a pw, allocates a message with a payload of @a payloadSize bytes
 * @param[in,out] pw Pointer to wagon in which to allocate the message (this pointer may be changed)
 * @param[in] payloadSize Size of the payload of the message to allocate
 * @return pointer on the allocated message (this function never returns NULL)
 * @warning OUTDATED -- @a mallocwiw is better !!
 */
message *mallocmsg_outdated(wagon **pw, int payloadSize);

/**
 * @brief Adds an application message to wagon @a w about arrival of process @a arrived
 * @param[in] w Wagon to which to add message
 * @param[in] arrived Process which arrived
 * @param[in] circuit Circuit in which process has arrived (@a arrived process can be mentionned in @a circuit, but it is not mandatory)
 */
void signalArrival_outdated(wagon *w, address arrived, address_set circuit); 

/**
 * @brief Adds one application message to wagon @a w per process which adress appears in @a departedSet
 * @param[in] w Wagon to which to add message(s)
 * @param[in] goneSet Set of gone process coded as one bit set per pne process
 * @param[in] circuit Circuit from which processes have gone (Processes in @a departedSet may not appear in @a circuit, but it is not mandatory)
 */
void signalDepartures_outdated(wagon *w, address_set goneSet, address_set circuit); 

#endif /* _wagon_H */
