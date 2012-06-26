/**
 * @brief Shows the fun which starts all the protocol
 * @file interface.h
 * @author Nathan REBOUD & Damien GRAUX
 * @date 07 june 2012
 */

#ifndef _INTERFACE_H
#define _INTERFACE_H

#include <semaphore.h>

/**
 * @brief The big semaphore used to manage the protocol in the function tr_init
 */
extern sem_t sem_init_done;

#endif /* _INTERFACE_H */
