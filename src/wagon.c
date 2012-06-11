// #define to access to definition of PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
// (Linux specific?)
#define _GNU_SOURCE

#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "bqueue.h"
#include "wagon.h"

t_bqueue *wagonsToDeliver = NULL;

message *firstmsg(wagon *w){
  if (w->header.len == sizeof(w->header))
    return NULL;
  else
    return w->msgs;
}

message *nextmsg(wagon *w, message *mp){
  message *mp2 = (message*)((char*)mp + mp->header.len);
  if ((char*)mp2 - (char*)w >= w->header.len)
    return NULL;
  else
    return mp2;
}
