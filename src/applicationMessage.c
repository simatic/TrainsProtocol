#include <string.h>
#include <stdio.h>
#include "applicationMessage.h"
#include "trains.h"
#include "wagon.h"
#include "counter.h"

#define ALONE 0 // FIXME
int automatonState = ALONE; //FIXME

CallbackCircuitChange theCallbackCircuitChange;
CallbackUtoDeliver    theCallbackUtoDeliver;

message *newmsg(int payloadSize){
  message *mp;
  MUTEX_LOCK(mutexWagonToSend);

  // We check that we have enough space for the message the caller wants to allocate
  while ((wagonToSend->header.len + sizeof(message_header) + payloadSize > WAGON_MAX_LEN)&&
	 (wagonToSend->header.len != sizeof(wagon_header))){
    int rc = pthread_cond_wait(&condWagonToSend, &mutexWagonToSend);
    if (rc < 0)								\
      error_at_line(EXIT_FAILURE,rc,__FILE__,__LINE__,"pthread_cond_wait"); \
  }

  mp = mallocmsg(&wagonToSend, payloadSize);
  mp->header.typ = AM_BROADCAST;

  // MUTEX_UNLOCK will be done in uto_broadcast
  // MUTEX_UNLOCK(mutexWagonToSend);
  //
  return mp;
}

int uto_broadcast(message *mp){
  if (automatonState == ALONE) {
    bqueue_enqueue(wagonsToDeliver, wagonToSend);
    wagonToSend = newwagon();
  }
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
static void fillCv(circuitview *cp, address_set circuit){
  address ad;
  memset(cp, 0, sizeof(*cp)); // Sets all the fields of cv to 0
  for (ad = 1; ad != 0; ad <<= 1){
    if (addr_ismember(ad, circuit)){
      cp->cv_members[cp->cv_nmemb] = ad;
      cp->cv_nmemb += 1;
    }
  }
}

void *uto_deliveries(void *null){
  wagon *w;
  message *mp;
  circuitview cv;
  bool terminate = false;

  do {
    w = bqueue_dequeue(wagonsToDeliver);

    counter_wagons_delivered++;

    // We analyze all messages in this wagon
    for (mp = firstmsg(w); mp != NULL ; mp = nextmsg(w, mp)) {

      counter_messages_delivered++;
      counter_messages_bytes_delivered += payload_size(mp);

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

    free(w);

  } while(!terminate);

  return NULL;
}
