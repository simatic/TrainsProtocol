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
extern int tr_errno;

/**
 * @brief Prints an error message

 * Same as gnu @a error_at_line function (see man error_at_line) except that 1) @a errnum is searched in trains middleware specific errors and 2) parameters behind @a format are not taken into account
 * @param[in] status same role as in @a error_at_line
 * @param[in] errnum same role as in @a error_at_line except that @a tr_error_at_line uses the string given by @a tr_perror(errnum)
 * @param[in] filename same role as in @a error_at_line
 * @param[in] linenum same role as in @a error_at_line
 * @param[in] format same role as in @a error_at_line
 */
void tr_error_at_line(int status, int errnum, const char *filename, unsigned int linenum, const char *format);

/**
 * @brief Initialization of trains protocol middleware
 * @param[in] callbackCircuitChange Function to be called when there is a circuit changed (Arrival or departure of a process)
 * @param[in] callbackUtoDeliver    Function to be called when a message can be uto-delivered by trains protocol
 * @return 0 upon successful completion, or -1 if an error occurred (in which case, @a tr_errno is set appropriately)
 */
int tr_init(CallbackCircuitChange callbackCircuitChange, CallbackUtoDeliver callbackUtoDeliver);

/**
 * @brief Prints (trains middleware specific) error message
 * @param[in] errnum Error number of the message to be printed
 */
void tr_perror(int errnum);

/**
 * @brief Termination of trains protocol middleware
 * @return 0 upon successful completion, or -1 if an error occurred (in which case, @a tr_errno is set appropriately)
 */
int tr_terminate();

#endif /* _TRAINS_H_ */
