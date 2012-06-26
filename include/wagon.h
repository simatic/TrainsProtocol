/**
 * @brief Definitions related to wagons used in trains protocol.
 * @file wagon.h
 * @author Michel SIMATIC
 * @date 26 june 2012
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
 * @brief Bqueue containing the wagons (received from trains protocol) which can be delivered
 * to the application layer
 */
extern t_bqueue *wagonsToDeliver;

/**
 * @brief Search the first message contained in a wagon @a w
 * @param[in] w A pointer on the wagon in which to search the first message
 * @return A pointer on a first message or NULL if @a w contains no messages
 */
message *firstmsg(wagon *w);

/**
 * @brief Give the message following message @a mp in wagon @a w
 * @param[in] w A pointer on a wagon in which to search next message
 * @param[in] mp A pointer on a message after which to search for an other message
 * @return A pointer on a next message or NULL if @a w contains no other messages
 */
message *nextmsg(wagon *w, message *mp);

#endif /* _wagon_H */
