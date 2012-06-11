#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "common.h"
#include "applicationMessage.h"
#include "wagon.h"
#include "trains.h"
#include "stateMachine.h"
#include "advanced_struct.h"

#define FAKE_ADDRESS 0xABCD
#define FAKE_CIRCUIT 0x9876
#define DATA 0xFEDCBA98
#define SIZE1 42
#define SIZE2 64

void *functionThread1(void *null) {
  usleep(1000);
  printf("Purge wagonToSend_outdated");
  MUTEX_LOCK(mutexWagonToSend);
  free(wagonToSend);
  wagonToSend = newwiw();
  pthread_cond_signal(&condWagonToSend);
  MUTEX_UNLOCK(mutexWagonToSend);
  return NULL;
}

void compare(char *testType, bool result){
  printf("Testing %s...", testType);
  if (!result){
    printf("... KO\n");
    exit(EXIT_FAILURE);
  }
  printf("...OK\n");
}

void aCallbackCircuitChange(circuitview *cp){
  if (addr_isnull(cp->cv_departed)) {
    // A process joined
    compare("uto_deliveries (callbackCircuitChange) test1",
	    (cp->cv_joined == 0x0001) &&
	    (cp->cv_nmemb == 3) &&
	    (cp->cv_members[0] == 0x0001) &&
	    (cp->cv_members[1] == 0x0002) &&
	    (cp->cv_members[2] == 0x0004));
  } else {
    // A process left
    if (cp->cv_departed == 0x0002) {
      compare("uto_deliveries (callbackCircuitChange) test2",
	      (cp->cv_nmemb == 1) &&
	      (cp->cv_members[0] == 0x0001));
    } else {
      compare("uto_deliveries (callbackCircuitChange) test3",
	      (cp->cv_nmemb == 1) &&
	      (cp->cv_members[0] == 0x0001));
    }
  }
}

void aCallbackUtoDeliver(address sender, message *mp){
      compare("uto_deliveries (callbackUtoDeliver)",
	      (sender == FAKE_ADDRESS) &&
	      (mp->header.len == sizeof(message_header) + sizeof(int)) &&
	      (*((int*)(mp->payload)) == DATA));
}

int main(){
  message *mp1, *mp2, *mp;
  int rc;
  wiw *w;
  pthread_t thread;

  // Miscellaneous initializations
  rc = pthread_cond_init(&condWagonToSend, NULL);
  assert(rc == 0);
  wagonsToDeliver = bqueue_new();

  // Test newwagon_outdated
  my_address = FAKE_ADDRESS;
  wagonToSend = newwiw();
  compare("newwagon_outdated",
	  (wagonToSend->p_wagon->header.len == sizeof(wagon_header)) &&
	  (wagonToSend->p_wagon->header.sender == FAKE_ADDRESS));

  // Test newmsg (which calls mallocmsg_outdated)
  mp1 = newmsg(SIZE1);
  MUTEX_UNLOCK(mutexWagonToSend);
  mp2 = newmsg(SIZE2);
  MUTEX_UNLOCK(mutexWagonToSend);
  compare("newmsg (which calls mallocmsg_outdated) test1",
	  (mp1->header.len == sizeof(message_header)+SIZE1) &&
	  ((unsigned int)mp1 - (unsigned int)wagonToSend->p_wagon ==
	   sizeof(wagon_header)));
  compare("newmsg (which calls mallocmsg_outdated) test2",
	  (mp2->header.len == sizeof(message_header)+SIZE2) &&
	  ((unsigned int)mp2 - (unsigned int)mp1 ==
	   sizeof(message_header) + SIZE1));

  // Test firstmsg
  w = newwiw();
  mp = firstmsg(w->p_wagon);
  compare("firstmsg test1", mp == NULL);
  free_wiw(w);
  mp = firstmsg(wagonToSend->p_wagon);
  compare("firstmsg test2", mp == mp1);

  // Test nextmsg
  mp = nextmsg(wagonToSend->p_wagon, mp);
  compare("nextmsg test1", mp == mp2);
  mp = nextmsg(wagonToSend->p_wagon, mp);
  compare("nextmsg test2", mp == NULL);

  // Test signalArrival
  signalArrival(FAKE_ADDRESS, FAKE_CIRCUIT);
  mp = firstmsg(wagonToSend->p_wagon);
  mp = nextmsg(wagonToSend->p_wagon, mp);
  mp = nextmsg(wagonToSend->p_wagon, mp);
  compare("signalArrival (which tests signalArrivalDepartures)",
	  (mp->header.typ == AM_ARRIVAL) &&
	  (((payloadArrivalDeparture*)(mp->payload))->ad == FAKE_ADDRESS) &&
	  (((payloadArrivalDeparture*)(mp->payload))->circuit == (FAKE_CIRCUIT|FAKE_ADDRESS)));

  // Test signalDepartures
  signalDepartures(0x0003, FAKE_CIRCUIT);
  mp = nextmsg(wagonToSend->p_wagon, mp);
  mp2 = nextmsg(wagonToSend->p_wagon, mp);
  compare("signalDeparturesArrival_outdated (which tests signalArrivalDepartures_outdated)",
	  (mp->header.typ == AM_DEPARTURE) &&
	  (((payloadArrivalDeparture*)(mp->payload))->ad == 0x0001) &&
	  (((payloadArrivalDeparture*)(mp->payload))->circuit == (FAKE_CIRCUIT&~0x0003))&&
	  (mp2->header.typ == AM_DEPARTURE) &&
	  (((payloadArrivalDeparture*)(mp2->payload))->ad == 0x0002) &&
	   (((payloadArrivalDeparture*)(mp2->payload))->circuit == (FAKE_CIRCUIT&~0x0003)));

  // Test some more newmsg (which calls mallocwiw)
  rc = pthread_create(&thread, NULL, &functionThread1, NULL);
  if (rc < 0)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  rc = pthread_detach(thread);
  if (rc < 0)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
  mp = newmsg(WAGON_MAX_LEN); // This message is too big to fit into the current wagon. We must wait till the wagon is made empty.
  MUTEX_UNLOCK(mutexWagonToSend);
  compare("newmsg (which calls mallocmsg_outdated) test3",
	  mp == firstmsg(wagonToSend->p_wagon));

  rc = pthread_create(&thread, NULL, &functionThread1, NULL);
  if (rc < 0)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  rc = pthread_detach(thread);
  if (rc < 0)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
  mp = newmsg(SIZE1); // This message is too big to fit into the current wagon. We must wait till the wagon is made empty.
  MUTEX_UNLOCK(mutexWagonToSend);
  compare("newmsg (which calls mallocmsg_outdated) test4",
	  mp == firstmsg(wagonToSend->p_wagon));

  // Test uto_broadcast when automatonState is ALONE_INSERT_WAIT
  automatonState = ALONE_INSERT_WAIT;
  uto_broadcast(mp);
  w = bqueue_dequeue(wagonsToDeliver);
  mp2 = firstmsg(w->p_wagon);
  compare("uto_broadcast (when ALONE)",
	  (mp == mp2) &&
	  (firstmsg(wagonToSend->p_wagon) == NULL));

  // Test uto_deliveries
  theCallbackCircuitChange = aCallbackCircuitChange;
  theCallbackUtoDeliver = aCallbackUtoDeliver;

  my_address = FAKE_ADDRESS;

  free_wiw(wagonToSend);
  wagonToSend = newwiw();
  signalArrival(0x0001, 0x0007);
  signalDepartures(0x0006, 0x0001);
  bqueue_enqueue(wagonsToDeliver, wagonToSend);

  wagonToSend = newwiw();
  mp = newmsg(sizeof(int));
  MUTEX_UNLOCK(mutexWagonToSend);
  *((int*)(mp->payload)) = DATA;
  mp = newmsg(0);
  MUTEX_UNLOCK(mutexWagonToSend);
  mp->header.typ = AM_TERMINATE;
  bqueue_enqueue(wagonsToDeliver, wagonToSend);

  uto_deliveries(NULL);

  return EXIT_SUCCESS;
}
