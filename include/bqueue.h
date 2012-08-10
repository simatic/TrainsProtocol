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
 * @brief Blocking queue: a queue for which, when a thread calls @a bqueueDequeue on an empty queue, this thread is blocked until another thread calls \a bqueue_queue to queue an element.
 * @file bqueue.h
 * @author Michel SIMATIC
 * @date  14 april 2012
 */

#ifndef _BQUEUE_H_
#define _BQUEUE_H_

#include <semaphore.h>
#include "list.h"

/** 
 * @brief Data structure holding a queue
 */
typedef struct{
  trList *list;   /**< List holding the different values in the queue */
  sem_t  readSem; /**< Semaphore used to determine how many values are present in \a list */
} trBqueue;

/**
 * @brief Creates a new bqueue
 * @return Pointer to the new bqueue
 */
trBqueue *newBqueue();

/**
 * @brief Removes the first element of @a aBQueue. In case \a aBQueue is empty, blocks the executing thread until an other thread adds an element to \a aBQueue.
 * @param[in] aBQueue Bqueue to work on
 * @return The first element of \a aBQueue
 */
void *bqueueDequeue(trBqueue *aBQueue);

/**
 * @brief Enqueues element @a anElt at the end of queue @a aBQueue
 * @param[in] aBQueue Bqueue to work on
 * @param[in] anElt Element to add to \a aBQueue
 */
void bqueueEnqueue(trBqueue *aBQueue, void *anElt);

/**
 * @brief Do several enqueue of elements of @a list in @a aBQueue
 * @param[in] aBQueue The pointer on the trBqueue wich is bouned to grow
 * @param[in] list The pointer on the list to enqueue
 */
void bqueueExtend(trBqueue *aBQueue, trList *list);

/**
 * @brief Frees @a aBQueue
 * @param[in] aBQueue List to work on
 * @warning 
 * <ol>
 * <li>If elements of @a aBQueue were pointers to allocated structures, these elements are not freed.</li>
 * <li>Freeing a \a bqueue that other threads are currently blocked on (in \a bqueueDequeue) produces undefined behavior.</li>
 * </ol>
 */
void freeBqueue(trBqueue *aBQueue);

#endif /* _BQUEUE_H_ */
