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
 * @brief Store the error number specific to errors in trains middleware
 */
extern int trErrno;

/**
 * @brief Prints an error message

 * Same as gnu @a error_at_line function (see man error_at_line) except that 1) @a errnum is searched in trains middleware specific errors and 2) parameters behind @a format are not taken into account
 * @param[in] status same role as in @a error_at_line
 * @param[in] errnum same role as in @a error_at_line except that @a trError_at_line uses the string given by @a trPerror(errnum)
 * @param[in] filename same role as in @a error_at_line
 * @param[in] linenum same role as in @a error_at_line
 * @param[in] format same role as in @a error_at_line
 */
void trError_at_line(int status, int errnum, const char *filename, unsigned int linenum, const char *format);

/**
 * @brief Initialization of trains protocol middleware
 * @param[in] callbackCircuitChange Function to be called when there is a circuit changed (Arrival or departure of a process)
 * @param[in] callbackUtoDeliver    Function to be called when a message can be uto-delivered by trains protocol
 * @return 0 upon successful completion, or -1 if an error occurred (in which case, @a trErrno is set appropriately)
 */
int trInit(CallbackCircuitChange callbackCircuitChange, CallbackUtoDeliver callbackUtoDeliver);

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
