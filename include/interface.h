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
 * @brief Shows the fun which starts all the protocol
 * @file interface.h
 * @author Nathan REBOUD & Damien GRAUX
 * @date 07 june 2012
 */

#ifndef _INTERFACE_H
#define _INTERFACE_H

#include <semaphore.h>
#include <sys/stat.h>

/**
 * @brief The big semaphore used to manage the protocol in the function trInit
 */
#ifdef DARWIN
  // MacOS implements only named semaphores
  extern sem_t *sem_init_done;
#else
  extern sem_t sem_init_done;
#endif

void format_class_name(char *);

#endif /* _INTERFACE_H */
