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

#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include "signalArrival.h"


/**
 * @brief Adds one application message to wagon @a w for arrived or gone process @a ad (this is determined by @a typ variable)
 * @param[in] typ Type of message to add in @a header.typ of the message
 * @param[in] ad Address of arrived or gone process
 * @param[in] circuit Circuit into (respectively from) which process come (respectively left)
*/
static void signalArrivalDepartures(char typ, address ad, addressSet circuit){
  message *mp;
  MUTEX_LOCK(mutexWagonToSend);

  // This function is called by thread processing the receiving of a train.
  // Thus, we must not call newmsg() as newmsg could get stuck when asking
  // for space in wagonToSend in the case the wagon is already full. If
  // newmsg was stuck, we would be in a dead lock (as only the thread processing
  // the train is able to empty wagonToSend).

  // We must send a msg even if the wagon is full. Thus, if there is not enough
  // space in the wagon, we realloc it.
  if ((wagonMaxLen - wagonToSend->p_wagon->header.len)
      < (int) sizeof(payloadArrivalDeparture)) {
    int newWomimLen = sizeof(prefix) + wagonToSend->p_wagon->header.len
        + sizeof(messageHeader) + sizeof(payloadArrivalDeparture);
    wagonToSend->p_womim = realloc(wagonToSend->p_womim, newWomimLen);
    assert(wagonToSend->p_womim != NULL);
  }

  mp = mallocWiw(sizeof(payloadArrivalDeparture));

  mp->header.typ = typ;
  ((payloadArrivalDeparture*)(mp->payload))->ad = ad;
  ((payloadArrivalDeparture*)(mp->payload))->circuit = circuit;

  MUTEX_UNLOCK(mutexWagonToSend);
}

void signalArrival(address arrived, addressSet circuit){
  circuit |= arrived;
  signalArrivalDepartures(AM_ARRIVAL, arrived, circuit);
}

void signalDepartures(addressSet goneSet, addressSet circuit){
  address ad;
  if (goneSet != 0){
	  circuit &= ~goneSet;
	  for (ad = 1; ad != 0; ad <<= 1) {
		  if (addrIsMember(ad, goneSet)) {
			  signalArrivalDepartures(AM_DEPARTURE, ad, circuit);
		  }
	  }
  }
}
