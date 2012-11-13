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
 * @brief Declarations common to all files
 * @file common.h
 * @author Michel SIMATIC
 * @date  15 april 2012
 */

#ifndef _COMMON_H
#define _COMMON_H

#include <stdlib.h>
#include <pthread.h>
//#include <error.h>

/**
 * @brief Enumeration used to define booleans.
 */
typedef enum {false, true} bool;

/**
 * @brief Macro to lock a mutex (and check that everything went right)
 */
#define MUTEX_LOCK(m) \
  {						      \
    int rc=pthread_mutex_lock(&(m));		      \
    if (rc < 0)								\
      error_at_line(EXIT_FAILURE,rc,__FILE__,__LINE__,"pthread_mutex_lock"); \
  }

/**
 * @brief Macro to unlock a mutex (and check that everything went right)
 */
#define MUTEX_UNLOCK(m) \
  {						      \
    int rc=pthread_mutex_unlock(&(m));					\
    if (rc < 0)								\
      error_at_line(EXIT_FAILURE,rc,__FILE__,__LINE__,"pthread_mutex_lock"); \
  }

/**
 * @brief Macro to destroy a mutex (and check that everything went right)
 */
#define MUTEX_DESTROY(m) \
  {						      \
    int rc=pthread_mutex_destroy(&(m));					\
    if (rc < 0)								\
      error_at_line(EXIT_FAILURE,rc,__FILE__,__LINE__,"pthread_mutex_destroy"); \
  }


#endif /* _COMMON_H_ */
