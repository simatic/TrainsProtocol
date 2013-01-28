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

Developer(s): Michel Simatic, Arthur Foltz, Damien Graux, Nicolas Hascoet, Stephanie Ouillon, Nathan Reboud
*/

#include <string.h>
#include <stdio.h>
#include "trains.h"
#include "wagon.h"
#include "msg.h"
#include "advanced_struct.h"
#include "counter.h"
#include "stateMachine.h"
#include "errorTrains.h"
#include "trains_Interface.h"
#include "jniContext.h"

/* Global JNI variables */
jobject jcallbackUtoDeliver;
jobject jcallbackCircuitChange;
jmethodID jcallbackUtoDeliver_runID;
jmethodID jcallbackCircuitChange_runID;

JNIEXPORT jint JNICALL Java_trains_Interface_newmsg(JNIEnv *env, jobject obj, jint payloadSize, jbyteArray payload){
  message *mp;
  char *buf;
  
  buf = malloc(payloadSize*sizeof(char));
  
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

  mp = mallocWiw(payloadSize);
  mp->header.typ = AM_BROADCAST;

  /* The content of the message is filled at these two lines 
   * We get the content of payload in buf, and then we memcopy it in mp->payload */
  (*env)->GetByteArrayRegion(env, payload, 0, payloadSize, (jbyte *) buf);

   memcpy(mp->payload, buf, payloadSize);

  // MUTEX_UNLOCK will be done in utoBroadcast
  // MUTEX_UNLOCK(mutexWagonToSend);
  //

  return 0;
}

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
  int i=0;

  jclass class;
  jmethodID mid = NULL;
  jobject jobj;
  char destUto[255];
  char destCC[255];
  jbyteArray msgPayload_temp;
  jbyteArray msgPayload;

  /* Get the JNIenv pointer
   * This operation is necessary because we want to perform actions
   * in the same Java Virtual Machine that initializes the trains protocol
   * in trInit() - which was done in another thread, so we had to cache
   * the JVM in the variable jvm in trInit() and here we get the JNIenv pointer from it. */
  JNIEnv *JNIenv;
  (*jvm)->AttachCurrentThread(jvm, (void **)&JNIenv, NULL);
  if (*JNIenv == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "attach the current thread to the JVM");
  }

  /* Callbacks : get methods IDs and instantiate objects  */
  class = (*JNIenv)->FindClass(JNIenv, theJNICallbackUtoDeliver);
  /*jthrowable exec;
  exec = (*JNIenv)->ExceptionOccurred(JNIenv);
  if (exec) {
    jclass newExcCls;
    (*JNIenv)->ExceptionDescribe(JNIenv);
    (*JNIenv)->ExceptionClear(JNIenv);
  }*/
  if (class == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find class implementing CallbackUtoDeliver");
  }
  
  strcat(destUto, "()L");
  strcat(destUto, theJNICallbackUtoDeliver);
  strcat(destUto, ";");  

  mid = (*JNIenv)->GetStaticMethodID(JNIenv, class, "getInstance", destUto);
  if (mid == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find callbackCircuitChange getInstance()");
  }  
  
  jobj = (*JNIenv)->CallStaticObjectMethod(JNIenv, class, mid);
  if (jobj == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "call static factory for callbackCircuitChange");
  }
  
  jcallbackUtoDeliver = (*JNIenv)->NewGlobalRef(JNIenv, jobj);
  if(jcallbackUtoDeliver == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "Global ref for CircuitView");
  }
  (*JNIenv)->DeleteLocalRef(JNIenv, jobj);
  
  jcallbackUtoDeliver_runID = (*JNIenv)->GetMethodID(JNIenv, class, "run", "(ILtrains/Message;)V");
  if (jcallbackUtoDeliver_runID == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "get method ID for running callbackUtoDeliver");
  }  
    
  class = (*JNIenv)->FindClass(JNIenv, theJNICallbackCircuitChange);
  if (class == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find class implementing CallbackCircuitChange");
  }

  strcat(destCC, "()L");
  strcat(destCC, theJNICallbackCircuitChange);
  strcat(destCC, ";");  

  mid = (*JNIenv)->GetStaticMethodID(JNIenv, class, "getInstance", destCC);
  if (mid == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find callbackCircuitChange getInstance()");
  }  

  jobj = (*JNIenv)->CallStaticObjectMethod(JNIenv, class, mid);
  if (jobj == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "call static factory for callbackCircuitChange");
  }

  jcallbackCircuitChange = (*JNIenv)->NewGlobalRef(JNIenv, jobj);
  if(jcallbackCircuitChange == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "Global ref for CircuitView");
  }
  (*JNIenv)->DeleteLocalRef(JNIenv, jobj);
  
  jcallbackCircuitChange_runID = (*JNIenv)->GetMethodID(JNIenv, class, "run", "(Ltrains/CircuitView;)V");
  if (jcallbackUtoDeliver_runID == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "get method ID for running callbackUtoDeliver");
  }  
    
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
        {   
	        /* Set message header */
          (*JNIenv)->SetIntField(JNIenv, jmsghdr, jmsghdr_lenID, mp->header.len); 
          (*JNIenv)->SetIntField(JNIenv, jmsghdr, jmsghdr_typeID, mp->header.typ); 

	        /* Set message */
          (*JNIenv)->SetObjectField(JNIenv, jmsg, jmsg_hdrID, jmsghdr); 
          
          msgPayload_temp = (*JNIenv)->NewByteArray(JNIenv, mp->header.len);
          if(msgPayload_temp == NULL){
            ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__,"msgPayload");
          }
          msgPayload = (*JNIenv)->NewGlobalRef(JNIenv, msgPayload_temp);
          if(msgPayload == NULL){
            ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "Global ref for msgPayload");
          }
          (*JNIenv)->DeleteLocalRef(JNIenv, msgPayload_temp);
          
          (*JNIenv)->SetByteArrayRegion(JNIenv, msgPayload, 0, payloadSize(mp), (jbyte *)mp->payload); 
          (*JNIenv)->SetObjectField(JNIenv, jmsg, jmsg_payloadID, msgPayload); 
          
          /* Call callback */
	        (*JNIenv)->CallVoidMethod(JNIenv, jcallbackUtoDeliver, jcallbackUtoDeliver_runID, w->header.sender, jmsg);
             
          break;
        }
        case AM_ARRIVAL:
          fillCv(&cv, ((payloadArrivalDeparture*) (mp->payload))->circuit);
          cv.cv_joined = ((payloadArrivalDeparture*) (mp->payload))->ad;
          
          (*JNIenv)->SetIntField(JNIenv, jcv, jcv_nmembID, cv.cv_nmemb); 
          
          /* Set CircuitView */
          for(i=0; i< MAX_MEMB; i++){
            (*JNIenv)->CallVoidMethod(JNIenv, jcv, jcv_setMembersAddressID, i, cv.cv_members[i]);
          }            
          
          (*JNIenv)->SetIntField(JNIenv, jcv, jcv_joinedID, cv.cv_joined); 
          (*JNIenv)->SetIntField(JNIenv, jcv, jcv_departedID, cv.cv_departed); 
          
          (*JNIenv)->CallVoidMethod(JNIenv, jcallbackCircuitChange, jcallbackCircuitChange_runID, jcv);
          break;
        case AM_DEPARTURE:
          fillCv(&cv, ((payloadArrivalDeparture*) (mp->payload))->circuit);
          cv.cv_departed = ((payloadArrivalDeparture*) (mp->payload))->ad;
          
          (*JNIenv)->SetIntField(JNIenv, jcv, jcv_nmembID, cv.cv_nmemb); 
          
          /* Set CircuitView */
          for(i=0; i< MAX_MEMB; i++){
            (*JNIenv)->CallVoidMethod(JNIenv, jcv, jcv_setMembersAddressID, i, cv.cv_members[i]);
          }            
          (*JNIenv)->SetIntField(JNIenv, jcv, jcv_joinedID, cv.cv_joined); 
          (*JNIenv)->SetIntField(JNIenv, jcv, jcv_departedID, cv.cv_departed); 
          
          (*JNIenv)->CallVoidMethod(JNIenv, jcallbackCircuitChange, jcallbackCircuitChange_runID, jcv);
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

  return NULL ;
}
