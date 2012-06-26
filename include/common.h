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
#include <error.h>

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
