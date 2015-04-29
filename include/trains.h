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
 * @brief Definitions offered to application by the trains middleware.
 * @file trains.h
 * @author Michel SIMATIC
 * @date 26 june 2012
 */

#ifndef _TRAINS_H
#define _TRAINS_H

#include "common.h"
#include "address.h"
#include "applicationMessage.h"

/**
 * @brief Localisation of the file where addresses are written
 */
#define LOCALISATION "./addr_file"/**<File's here.*/

/**
 * @brief The maximum number of rounds used for the modulos
 */
#define NR 3

/**
 * @brief The limit number of processes in the circuit
 * @note Set at 16
 */
#define MAX_NTR 16

/**
 * @brief Time to wait
 */
#define CONNECT_TIMEOUT 2000

/**
 * @brief Number of train allowed in the circuit
 * @note The number should be adjust with care to make the protocol as performant as possible
 */
extern int ntr;

/**
 * @brief Max size of a wagon
 */
 extern int wagonMaxLen;

/**
 * @brief The limit of time to wait
 */
extern int waitNbMax;

/**
 * @brief The default time to wait
 */
extern int waitDefaultTime; /**<in microsecond*/

/**
 * @brief Store the error number specific to errors in trains middleware
 */
extern int trErrno;

/**
 * @brief Prints an error message

 * Same as gnu @a error_at_line function (see man error_at_line) except that 1) @a errnum is searched in trains middleware specific errors and 2) parameters behind @a format are not taken into account
 * @param[in] status same role as in @a ERROR_AT_LINE
 * @param[in] errnum same role as in @a ERROR_AT_LINE except that @a trError_at_line uses the string given by @a trPerror(errnum)
 * @param[in] filename same role as in @a ERROR_AT_LINE
 * @param[in] linenum same role as in @a ERROR_AT_LINE
 * @param[in] format same role as in @a ERROR_AT_LINE
 */
void trError_at_line(int status, int errnum, const char *filename, unsigned int linenum, const char *format);

/**
 * \enum t_reqOrder
 * \brief Possible orders which can be required from Trains algorithm
 */

typedef enum 
{
    CAUSAL_ORDER        = 0, /**< Request for causal order only. */
    TOTAL_ORDER         = 1, /**< Request for total order, without uniformity guarantees (a message can be delivered to a process which happens to fail ; and, this message may be never be delivered to other participating processes). */
    UNIFORM_TOTAL_ORDER = 2 /**< Request for uniform total order. If a message is delivered to a process, it is guaranteed that all correct processes will deliver this message. */
} t_reqOrder;

/**
 * @brief Number of rounds which must be made by a train before delivery (this value depends of requiredOrder)
 */
 extern int nbRounds;

/**
 * @brief Initialization of trains protocol middleware

 * @param[in] trainsNumber The number of trains on the circuit
 * @param[in] wagonLength The length of the wagons in the trains
 * @param[in] waitNb The number of time to wait
 * @param[in] waitTime The time to wait (in microsecond)
 * @param[in] callbackCircuitChange Function to be called when there is a circuit changed (Arrival or departure of a process)
 * @param[in] callbackODeliver    Function to be called when a message can be o-delivered by trains protocol
 * @param[in] reqOrder Order guarantees which Trains algorithm must provide while it is running
 * @return 0 upon successful completion, or -1 if an error occurred (in which case, @a trErrno is set appropriately)
 */
int trInit(int trainsNumber, int wagonLength, int waitNb, int waitTime, CallbackCircuitChange callbackCircuitChange, CallbackODeliver callbackODeliver, t_reqOrder reqOrder);

/**
 * @brief Contains the required order specified with @a reqOrder parameter in @a trInit function
 */
extern t_reqOrder requiredOrder;

/**
 * @brief Prints (trains middleware specific) error message
 * @param[in] errnum Error number of the message to be printed
 */
void trPerror(int errnum);

/**
 * @brief Termination of trains protocol middleware
 * @return 0 upon successful completion, or -1 if an error occurred (in which case, @a trErrno is set appropriately)
 */
int trTerminate();

#endif /* _TRAINS_H_ */
