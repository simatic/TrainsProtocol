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
#include "errorTrains.h"
#include <jni.h>
#include "trains_Interface.h"


JNIEXPORT jint JNICALL Java_trains_Interface_newmsg(JNIEnv *env, jobject obj, jint payloadSize){
  //message *mp;
  counters.newmsg++;
  MUTEX_LOCK(mutexWagonToSend);

  // We check that we have enough space for the message the caller wants to allocate
  while ((wagonToSend->p_wagon->header.len + sizeof(messageHeader) + payloadSize
        > wagonMaxLen)
      && (wagonToSend->p_wagon->header.len != sizeof(wagonHeader))) {
    counters.flowControl++;
    int rc = pthread_cond_wait(&condWagonToSend, &mutexWagonToSend);
    if (rc < 0)
      ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_cond_wait");
  }

  //mp = mallocWiw(payloadSize);
  //mp->header.typ = AM_BROADCAST;

  // MUTEX_UNLOCK will be done in utoBroadcast
  // MUTEX_UNLOCK(mutexWagonToSend);
  //
  //return mp;
  return 0;
}

//int utoBroadcast(message *mp){
JNIEXPORT jint JNICALL Java_trains_Interface_utoBroadcast(JNIEnv *env, jobject obj, jobject msg){ 
 //  MUTEX_LOCK(stateMachineMutex); // We DO NOT take this mutex
  // state ALONE_INSERT_WAIT => ALONE_CONNECTION_WAIT require mutexWagonToSend
  // thus cannot corrupt this sending
  // state SEVERAL => ALONE_INSERT_WAIT also require mutexWagonToSend

  if (automatonState == ALONE_INSERT_WAIT) {
    bqueueEnqueue(wagonsToDeliver, wagonToSend);
    wagonToSend = newWiw();
  }

  //See comment above
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
  for (ad = 1; ad != 0; ad <<= 1) {
    if (addrIsMember(ad, circuit)) {
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

  /* Create the JVM */
  /*JavaVMOption options[1];
  JavaVm *jvm;
  JavaVMInitArgs vm_args;
  long status;*/
  jclass cls;
  jmethodID mid;

  /*Java objects */
  jobject jmsg_hdr;
  jobject jmsg;
  jobject jcircuit_view;
  jobject jcallbackCircuitChange;
  jobject jcallbackUtoDeliver;

  /* Java methods IDs*/
  /* Callbacks */
  jmethodID jcircuitChangeId;
  jmethodID jutoDeliverId;
 
  /* Setters */
  jmethodID jmsg_setMessageHeaderId;
  jmethodID jmsg_setPayloadId;
  jmethodID jmsghdr_setLenId;
  jmethodID jmsghdr_setTypeId;
  jmethodID jcv_setMembId;
  jmethodID jcv_setMembersAddressId;
  jmethodID jcv_setJoinedId;
  jmethodID jcv_setDepartedId;


  /* Start the JVM */
  /* No need to start the jvm, we got it from JNIenv */
  /* options[0].optionString = "-Djava.class.path=/Users/stephanie/dev/PFE/TrainsJNI/src/bin/trains"; //XXX: set the path
  memset(&vm_args, 0, sizeof(vm_args));
  vm_args.version = JNI_VERSION_1_2;
  vm_args.nOptions = 1;
  vm_args.options = options;
  status = JNI_CreateJavaVM(&jvm, (void**)&JNIenv, &vm_args); */

  /* Get the JNIenv pointer*/
  JNIEnv *JNIenv;
  (*jvm)->AttachCurrentThread(jvm, (void **)&JNIenv, NULL);
  /* Instantiate Java objects: MessageHeader, Message and CircuitView */
  cls = (*JNIenv)->FindClass(JNIenv, "trains/MessageHeader");
  if (cls != 0){
    mid = (*JNIenv)->GetMethodID(JNIenv, cls, "MessageHeader", "(LMessageHeader)II");
    if(mid != 0){      
      jmsg_hdr = (*JNIenv)->NewObject(JNIenv, cls, mid, 0, 0);
    }
  }
  
  if(jmsg_hdr == 0){
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jmsg_hdr");
  }

  cls = (*JNIenv)->FindClass(JNIenv, "trains/Message");
  if (cls != 0){
    mid = (*JNIenv)->GetMethodID(JNIenv, cls, "Message", "(LMessage)SV");
    if(mid != 0){      
      jmsg = (*JNIenv)->NewObject(JNIenv, cls, mid, "my_string", jmsg);
    }
  }
  
  if(jmsg == 0){
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jmsg");
  }
  
  cls = (*JNIenv)->FindClass(JNIenv, "trains/CircuitView");
  if (cls != 0){
    mid = (*JNIenv)->GetMethodID(JNIenv, cls, "CircuitView", "(LCircuitView)IIII");
    if(mid != 0){      
      jcircuit_view = (*JNIenv)->NewObject(JNIenv, cls, mid, 0, 0, 0, 0);
    }
  }
  
  if(jcircuit_view == 0){
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jcircuit_view");
  }

  /* Get java methods IDs */

  /* Callbacks : get methods IDs and instantiate objects  */
  cls = (*JNIenv)->FindClass(JNIenv, theJNICallbackUtoDeliver); //XXX: check it is a correct string
  if (cls != 0){
    jutoDeliverId = (*JNIenv)->GetMethodID(JNIenv, cls, "run", "(V)ILMessage");
    //mid = (*JNIenv)->GetMethodID(JNIenv, cls, "", "(V)ILMessage"); //XXX: be careful with the prototype, to be changed
  }
 
  if(jutoDeliverId == 0){
  /*  ERROR_AT_LINE();
  }
  if(mid == 0){*/
  
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "get jutoDeliverId");
  } else {
    jcallbackUtoDeliver = (*JNIenv)->NewObject(JNIenv, cls, jutoDeliverId);
    if(jcallbackUtoDeliver == 0){
      ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jcallbackUtoDeliver");
    }
  }


  cls = (*JNIenv)->FindClass(JNIenv, theJNICallbackCircuitChange); //XXX: check it is a correct string
  if (cls != 0){
    jcircuitChangeId = (*JNIenv)->GetMethodID(JNIenv, cls, "run", "(V)LCircuitView");
    //mid = (*JNIenv)->GetMethodID(JNIenv, cls, cls, "(V)LCircuitView"); //XXX: be careful with the prototype, to be changed
  }

  if(jcircuitChangeId == 0){
  /*  ERROR_AT_LINE();
  }
  if(mid == 0){*/
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jcircuitChangeId");
  } else {
    jcallbackCircuitChange = (*JNIenv)->NewObject(JNIenv, cls, jcircuitChangeId);
    if(jcallbackCircuitChange == 0){
      ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jcallbackCircuitChange");
    }
  }
  
  /* Setters */
  cls = (*JNIenv)->FindClass(JNIenv, "trains/Message");
  if (cls != 0){
    jmsg_setMessageHeaderId = (*JNIenv)->GetMethodID(JNIenv, cls, "setMessageHeader", "(V)LMessageHeader");
  } 
  if (jmsg_setMessageHeaderId == 0){
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jmsg_setMessageHeaderId");
  }
  
  cls = (*JNIenv)->FindClass(JNIenv, "trains/Message");
  if (cls != 0){
    jmsg_setPayloadId = (*JNIenv)->GetMethodID(JNIenv, cls, "setPayload", "(V)I");
  } 
  if (jmsg_setPayloadId == 0){
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jmsg_setPayloadId");
  }
  
  cls = (*JNIenv)->FindClass(JNIenv, "trains/MessageHeader");
  if (cls != 0){
    jmsghdr_setLenId = (*JNIenv)->GetMethodID(JNIenv, cls, "setLen", "(V)I");
  } 
  if (jmsghdr_setLenId == 0){
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jmsghdr_setLenId");
  }

  cls = (*JNIenv)->FindClass(JNIenv, "trains/MessageHeader");
  if (cls != 0){
    jmsghdr_setTypeId = (*JNIenv)->GetMethodID(JNIenv, cls, "setType", "(V)I");
  } 
  if (jmsghdr_setTypeId == 0){
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jmsghdr_setTypeId");
  }

  cls = (*JNIenv)->FindClass(JNIenv, "trains/CircuitView");
  if (cls != 0){
    jcv_setMembId = (*JNIenv)->GetMethodID(JNIenv, cls, "setMemb", "(V)I");
  } 
  if (jcv_setMembId == 0){
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jcv_setMembId");
  }

  cls = (*JNIenv)->FindClass(JNIenv, "trains/CircuitView");
  if (cls != 0){
    jcv_setMembersAddressId = (*JNIenv)->GetMethodID(JNIenv, cls, "setMembersAddress", "(V)I");
  } 
  if (jcv_setMembersAddressId == 0){
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jcv_setMemberAddressId");
  }

  cls = (*JNIenv)->FindClass(JNIenv, "trains/CircuitView");
  if (cls != 0){
    jcv_setJoinedId = (*JNIenv)->GetMethodID(JNIenv, cls, "setJoined", "(V)I");
  } 
  if (jcv_setJoinedId == 0){
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jcv_setJoinedId");
  }
  
  cls = (*JNIenv)->FindClass(JNIenv, "trains/CircuitView");
  if (cls != 0){
    jcv_setDepartedId = (*JNIenv)->GetMethodID(JNIenv, cls, "setDeparted", "(V)I");
  } 
  if (jcv_setDepartedId == 0){
    ERROR_AT_LINE(EXIT_FAILURE, 0, __FILE__, __LINE__, "instantiate jcv_setDepartedId");
  }

  //if (status != JNI_ERR){

  do {
    wi = bqueueDequeue(wagonsToDeliver);
    w = wi->p_wagon;

    counters.wagons_delivered++;

    // We analyze all messages in this wagon
    for (mp = firstMsg(w); mp != NULL ; mp = nextMsg(w, mp)) {

      counters.messages_delivered++;
      counters.messages_bytes_delivered += payloadSize(mp);

      switch (mp->header.typ) {
#ifdef LATENCY_TEST
        case AM_PING:
        case AM_PONG:
#endif /* LATENCY_TEST */
        case AM_BROADCAST:
            //(*theCallbackUtoDeliver)(w->header.sender, mp);
            //mp: type message
            //w->header.sender: type address (which is unsigned short)
           
	  /* Set message header */
          (*JNIenv)->CallVoidMethod(JNIenv, jmsg_hdr, jmsghdr_setLenId, mp->header.len); 
          (*JNIenv)->CallVoidMethod(JNIenv, jmsg_hdr, jmsghdr_setTypeId, mp->header.typ); 
	  /* Set message */
          (*JNIenv)->CallVoidMethod(JNIenv, jmsg, jmsg_setMessageHeaderId, jmsg_hdr); 
          //XXX: mp->payload is char[]
          (*JNIenv)->CallVoidMethod(JNIenv, jmsg, jmsg_setPayloadId, mp->payload); 
          
          /* Call callback */
          //give int w->header.sender directly ?
	  (*JNIenv)->CallVoidMethod(JNIenv, jcallbackUtoDeliver, jutoDeliverId, w->header.sender, jmsg);
             
          break;
        case AM_ARRIVAL:
          fillCv(&cv, ((payloadArrivalDeparture*) (mp->payload))->circuit);
          cv.cv_joined = ((payloadArrivalDeparture*) (mp->payload))->ad;
            //(*theCallbackCircuitChange)(&cv);
            //cv wich is a circuitView object (to be defined in Java)
          /* Set CircuitView */
          (*JNIenv)->CallVoidMethod(JNIenv, jcircuit_view, jcv_setMembId, cv.cv_nmemb); 
          (*JNIenv)->CallVoidMethod(JNIenv, jcircuit_view, jcv_setMembersAddressId, cv.cv_members[MAX_MEMB]); 
          (*JNIenv)->CallVoidMethod(JNIenv, jcircuit_view, jcv_setJoinedId, cv.cv_joined); 
          (*JNIenv)->CallVoidMethod(JNIenv, jcircuit_view, jcv_setDepartedId, cv.cv_departed); 
          
          (*JNIenv)->CallVoidMethod(JNIenv, jcallbackCircuitChange, jcircuitChangeId, jcircuit_view);
          break;
        case AM_DEPARTURE:
          fillCv(&cv, ((payloadArrivalDeparture*) (mp->payload))->circuit);
          cv.cv_departed = ((payloadArrivalDeparture*) (mp->payload))->ad;
            //(*theCallbackCircuitChange)(&cv);
            //mp->payload wich is char[]
          /* Set CircuitView */
          (*JNIenv)->CallVoidMethod(JNIenv, jcircuit_view, jcv_setMembId, cv.cv_nmemb); 
          (*JNIenv)->CallVoidMethod(JNIenv, jcircuit_view, jcv_setMembersAddressId, cv.cv_members[MAX_MEMB]); 
          (*JNIenv)->CallVoidMethod(JNIenv, jcircuit_view, jcv_setJoinedId, cv.cv_joined); 
          (*JNIenv)->CallVoidMethod(JNIenv, jcircuit_view, jcv_setDepartedId, cv.cv_departed); 
          
          (*JNIenv)->CallVoidMethod(JNIenv, jcallbackCircuitChange, jcircuitChangeId, jcircuit_view);
          break;
        case AM_TERMINATE:
          terminate = true;
          break;
        default:
          fprintf(stderr, "Received a message with unknown typ \"%d\"\n",
              mp->header.typ);
          break;
      }
    }
    freeWiw(wi);
  } while (!terminate);

  //}
  return NULL ;
}
