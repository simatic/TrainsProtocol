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

#include <string.h>
#include <stdio.h>
#include "applicationMessage.h"
#include "trains.h"
#include "wagon.h"
#include "msg.h"
#include "advanced_struct.h"
#include "counter.h"
#include "stateMachine.h"

CallbackCircuitChange theCallbackCircuitChange;
CallbackUtoDeliver    theCallbackUtoDeliver;

message *newmsg(int payloadSize){
  message *mp;
  counters.newmsg++;
  MUTEX_LOCK(mutexWagonToSend);

  // We check that we have enough space for the message the caller wants to allocate
  while ((wagonToSend->p_wagon->header.len + sizeof(messageHeader) + payloadSize > WAGON_MAX_LEN)&&
	 (wagonToSend->p_wagon->header.len != sizeof(wagonHeader))){
    counters.flowControl++;
    int rc = pthread_cond_wait(&condWagonToSend, &mutexWagonToSend);
    if (rc < 0)								\
      error_at_line(EXIT_FAILURE,rc,__FILE__,__LINE__,"pthread_cond_wait"); \
  }

  mp = mallocWiw(payloadSize);
  mp->header.typ = AM_BROADCAST;

  // MUTEX_UNLOCK will be done in utoBroadcast
  // MUTEX_UNLOCK(mutexWagonToSend);
  //
  return mp;
}

int utoBroadcast(message *mp){
  //  MUTEX_LOCK(stateMachineMutex);
  if (automatonState == ALONE_INSERT_WAIT) {
    bqueueEnqueue(wagonsToDeliver, wagonToSend);
    wagonToSend = newWiw();
  }
  //MUTEX_UNLOCK(stateMachineMutex);
  
  // Message is already in wagonToSend. All we have to do is to unlock the mutex.
  // If automatonState is in another state than ALONE, 
  MUTEX_UNLOCK(mutexWagonToSend);
  return 0;
}

/**
 * @brief Fill parts of @a cv common to arrival and departure of a process
 * @param[in,out] cp The variable to fill
 * @param[in] circuit Circuit to use for filling @a cv->cv_nmemb and @a cv->cv_members
 */
static void fillCv(circuitView *cp, addressSet circuit){
  address ad;
  memset(cp, 0, sizeof(*cp)); // Sets all the fields of cv to 0
  for (ad = 1; ad != 0; ad <<= 1){
    if (addrIsMember(ad, circuit)){
      cp->cv_members[cp->cv_nmemb] = ad;
      cp->cv_nmemb += 1;
    }
  }
}

void *utoDeliveries(void *null){
  wiw *wi;
  wagon *w;
  message *mp;
  circuitView cv;
  bool terminate = false;

  do {
    wi = bqueueDequeue(wagonsToDeliver);
    w = wi->p_wagon;
  
    counters.wagons_delivered++;
      
    // We analyze all messages in this wagon
    for (mp = firstMsg(w); mp != NULL ; mp = nextMsg(w, mp)) {
	
      counters.messages_delivered++;
      counters.messages_bytes_delivered += payloadSize(mp);
	
      switch (mp->header.typ) {
      case AM_BROADCAST:
	(*theCallbackUtoDeliver)(w->header.sender, mp);
	break;
      case AM_ARRIVAL:
	fillCv(&cv, ((payloadArrivalDeparture*)(mp->payload))->circuit);
	cv.cv_joined = ((payloadArrivalDeparture*)(mp->payload))->ad;
	(*theCallbackCircuitChange)(&cv);
	break;
      case AM_DEPARTURE:
	fillCv(&cv, ((payloadArrivalDeparture*)(mp->payload))->circuit);
	cv.cv_departed = ((payloadArrivalDeparture*)(mp->payload))->ad;
	(*theCallbackCircuitChange)(&cv);
	break;
      case AM_TERMINATE:
	terminate = true;
	break;
      default:
	fprintf(stderr, "Received a message with unknown typ \"%d\"\n", mp->header.typ);
	break;
      }
    }
    freeWiw(wi);
  } while(!terminate);
  
  return NULL;
}
