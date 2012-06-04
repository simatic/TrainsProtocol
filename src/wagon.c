// #define to access to definition of PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
// (Linux specific?)
#define _GNU_SOURCE

#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "bqueue.h"
#include "wagon.h"

wagon *wagonToSend_outdated = NULL;

pthread_mutex_t mutexWagonToSend_outdated = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
pthread_cond_t condWagonToSend_outdated;

t_bqueue *wagonsToDeliver = NULL;


message *firstmsg(wagon *w){
  if (w->header.len == sizeof(w->header))
    return NULL;
  else
    return w->msgs;
}

message *mallocmsg_outdated(wagon **w, int payloadSize){
  message *mp;
  int newWagonLen = (*w)->header.len + sizeof(message_header) + payloadSize;
  *w = realloc(*w, newWagonLen);
  assert(*w != NULL);
  mp = (message*)((char*)*w + (*w)->header.len);
  mp->header.len = sizeof(message_header) + payloadSize;
  (*w)->header.len = newWagonLen;
  return mp;
}

message *nextmsg(wagon *w, message *mp){
  message *mp2 = (message*)((char*)mp + mp->header.len);
  if ((char*)mp2 - (char*)w >= w->header.len)
    return NULL;
  else
    return mp2;
}

wagon *newwagon_outdated(){
  wagon *w;
  w = malloc(sizeof(wagon_header));
  assert(w != NULL);
  w->header.len = sizeof(wagon_header);
  w->header.sender = my_address;
  w->header.round = 0;
  return w;
}

/**
 * @brief Adds one application message to wagon @a w for arrived or gone process @a ad (this is determined by @a typ variable)
 * @param[in] typ Type of message to add in @a header.typ of the message
 * @param[in] w Wagon to which to add message(s)
 * @param[in] ad Address of arrived or gone process
 * @param[in] circuit Circuit into (respectively from) which process come (respectively left)
 */
static void signalArrivalDepartures_outdated(char typ, wagon *w, address ad, address_set circuit){
  message *mp;
  MUTEX_LOCK(mutexWagonToSend_outdated);

  // This function is called by thread processing the receiving of a train.
  // Thus, we must not call newmsg() as newmsg could get stuck when asking
  // for space in wagonToSend in the case the wagon is already full. If
  // newmsg was stuck, we would be in a dead lock (as only the thread processing
  // the train is able to empty wagonToSend).
  mp = mallocmsg_outdated(&wagonToSend_outdated, sizeof(payloadArrivalDeparture));

  mp->header.typ = typ;
  ((payloadArrivalDeparture*)(mp->payload))->ad = ad;
  ((payloadArrivalDeparture*)(mp->payload))->circuit = circuit;

  MUTEX_UNLOCK(mutexWagonToSend_outdated);
}

void signalArrival_outdated(wagon *w, address arrived, address_set circuit){
  circuit |= arrived;
  signalArrivalDepartures_outdated(AM_ARRIVAL, w, arrived, circuit);
}

void signalDepartures_outdated(wagon *w, address_set goneSet, address_set circuit){
  address ad;
  circuit &= ~goneSet;
  for (ad = 1; ad != 0; ad <<= 1){
    if (addr_ismember(ad, goneSet)){
      signalArrivalDepartures_outdated(AM_DEPARTURE, w, ad, circuit);
    }
  }
}
