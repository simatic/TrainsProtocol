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

// #define to access to definition of PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
// (Linux specific?)
#define _GNU_SOURCE

#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "bqueue.h"
#include "wagon.h"

t_bqueue *wagonsToDeliver = NULL;

message *firstMsg(wagon *w){
  if (w->header.len == sizeof(w->header))
    return NULL;
  else
    return w->msgs;
}

message *nextMsg(wagon *w, message *mp){
  message *mp2 = (message*)((char*)mp + mp->header.len);
  if ((char*)mp2 - (char*)w >= w->header.len)
    return NULL;
  else
    return mp2;
}
