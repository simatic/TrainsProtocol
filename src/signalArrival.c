#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include "signalArrival.h"


/**
 * @brief Adds one application message to wagon @a w for arrived or gone process @a ad (this is determined by @a typ variable)
 * @param[in] typ Type of message to add in @a header.typ of the message
 * @param[in] w Wagon to which to add message(s)
 * @param[in] ad Address of arrived or gone process
 * @param[in] circuit Circuit into (respectively from) which process come (respectively left)
*/
static void signalArrivalDepartures(char typ, wagon *w, address ad, address_set circuit){
  message *mp;
  MUTEX_LOCK(mutexWagonToSend);

  // This function is called by thread processing the receiving of a train.
  // Thus, we must not call newmsg() as newmsg could get stuck when asking
  // for space in wagonToSend in the case the wagon is already full. If
  // newmsg was stuck, we would be in a dead lock (as only the thread processing
  // the train is able to empty wagonToSend).
  mp = mallocwiw(&wagonToSend, sizeof(payloadArrivalDeparture));

  mp->header.typ = typ;
  ((payloadArrivalDeparture*)(mp->payload))->ad = ad;
  ((payloadArrivalDeparture*)(mp->payload))->circuit = circuit;

  MUTEX_UNLOCK(mutexWagonToSend);
}

void signalArrival(wagon *w, address arrived, address_set circuit){
  circuit |= arrived;
  signalArrivalDepartures(AM_ARRIVAL, w, arrived, circuit);
}

void signalDepartures(wagon *w, address_set goneSet, address_set circuit){
  address ad;
  circuit &= ~goneSet;
  for (ad = 1; ad != 0; ad <<= 1){
    if (addr_ismember(ad, goneSet)){
      signalArrivalDepartures(AM_DEPARTURE, w, ad, circuit);
    }
  }
}
