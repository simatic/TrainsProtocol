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

  jclass class;
  jmethodID mid = NULL;
  jobject jobj;

  /* Get the JNIenv pointer*/
  JNIEnv *JNIenv;
  (*jvm)->AttachCurrentThread(jvm, (void **)&JNIenv, NULL);
  if (*JNIenv == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "attach the current thread to the JVM");
  }

  /*jclass stringClass = (*JNIenv)->FindClass(JNIenv, "java/lang/String");
     if (stringClass == NULL) {
         return NULL; *//* exception thrown */
    // }*/
 /* Get the method ID for the String(char[]) constructor */
   /*jmethodID cid = (*JNIenv)->GetMethodID(JNIenv, stringClass,
                               "<init>", "([C)V");
     if (cid == NULL) {
         return NULL;*/ /* exception thrown */
   // }
  /*jthrowable exec;
  exec = (JNIenv*)->ExceptionOccurred();
  if (exec) {
    jclass newExcCls;
    (JNIenv*)->ExceptionDescribe();
    (JNIenv*)->ExceptionClear();
  }*/

  /* Callbacks : get methods IDs and instantiate objects  */
  class = (*JNIenv)->FindClass(JNIenv, theJNICallbackUtoDeliver); //XXX: check it is a correct string
  jthrowable exec;
  exec = (*JNIenv)->ExceptionOccurred(JNIenv);
  if (exec) {
    jclass newExcCls;
    (*JNIenv)->ExceptionDescribe(JNIenv);
    (*JNIenv)->ExceptionClear(JNIenv);
  }
  if (class == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find class implementing CallbackUtoDeliver");
  }
  
  mid = (*JNIenv)->GetMethodID(JNIenv, class, "<init>", "()V");
  exec = (*JNIenv)->ExceptionOccurred(JNIenv);
  if (exec) {
    jclass newExcCls;
    (*JNIenv)->ExceptionDescribe(JNIenv);
    (*JNIenv)->ExceptionClear(JNIenv);
  }
  if (mid == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find callbackUtoDeliver constructor");
  }  
 
  jobj = (*JNIenv)->NewObject(JNIenv, class, mid);
  if (jobj == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "instantiate callbackUtoDeliver");
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
    
  class = (*JNIenv)->FindClass(JNIenv, theJNICallbackCircuitChange); //XXX: check it is a correct string
  if (class == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find class implementing CallbackCircuitChange");
  }
    mid = (*JNIenv)->GetMethodID(JNIenv, class, "<init>", "()V");
  if (mid == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find callbackCircuitChange constructor");
  }  
    
  jobj = (*JNIenv)->NewObject(JNIenv, class, mid);
  if (jobj == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "instantiate callbackCircuitChange");
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
    
//  if(mid == 0){*/
//    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "instantiate jcircuitChangeID");
//  } else {
//    jcallbackCircuitChange = (*JNIenv)->NewObject(JNIenv, class, jcircuitChangeID);
//    if(jcallbackCircuitChange == 0){
//      ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "instantiate jcallbackCircuitChange");
//    }
//  }
//


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
        {    //(*theCallbackUtoDeliver)(w->header.sender, mp);
            //mp: type message
            //w->header.sender: type address (which is unsigned short)
           
          printf("AM_BROADCAST\n");
	  /* Set message header */
          (*JNIenv)->SetIntField(JNIenv, jmsghdr, jmsghdr_lenID, mp->header.len); 
          (*JNIenv)->SetIntField(JNIenv, jmsghdr, jmsghdr_typeID, mp->header.typ); 

	  /* Set message */
          (*JNIenv)->SetObjectField(JNIenv, jmsg, jmsg_hdrID, jmsghdr); 
          //XXX: mp->payload is char[]
          // We want to convert a char* to a jstring

          /*jclass strClass = (*JNIenv)->FindClass(JNIenv,"java/lang/String"); 
	  jmethodID ctorID = (*JNIenv)->GetMethodID(JNIenv, strClass, "<init>", "([BLjava/lang/String;)V");*/ 
          //This works for UTF-8 strings
          jstring encoding = (*JNIenv)->NewStringUTF(JNIenv, mp->payload); 

          /*jbyteArray bytes = (*JNIenv)->NewByteArray(JNIenv, strlen(mp->payload)); 
          (*JNIenv)->SetByteArrayRegion(JNIenv, bytes, 0, strlen(mp->payload), (jbyte*)mp->payload); 
          jstring str = (jstring)(*JNIenv)->NewObject(JNIenv, strClass, ctorID, bytes, encoding);*/

          (*JNIenv)->SetObjectField(JNIenv, jmsg, jmsg_payloadID, encoding); 
          
          /* Call callback */
          //give int w->header.sender directly ?
	  (*JNIenv)->CallVoidMethod(JNIenv, jcallbackUtoDeliver, jcallbackUtoDeliver_runID, w->header.sender, jmsg);
             
          break;
        }
        case AM_ARRIVAL:
          fillCv(&cv, ((payloadArrivalDeparture*) (mp->payload))->circuit);
          cv.cv_joined = ((payloadArrivalDeparture*) (mp->payload))->ad;
            //(*theCallbackCircuitChange)(&cv);
            //cv wich is a circuitView object (to be defined in Java)
          /* Set CircuitView */
          printf("AM_ARRIVAL\n");
          (*JNIenv)->SetIntField(JNIenv, jcv, jcv_nmembID, cv.cv_nmemb); 
          (*JNIenv)->SetIntField(JNIenv, jcv, jcv_membersID, cv.cv_members[MAX_MEMB]); 
          (*JNIenv)->SetIntField(JNIenv, jcv, jcv_joinedID, cv.cv_joined); 
          (*JNIenv)->SetIntField(JNIenv, jcv, jcv_departedID, cv.cv_departed); 
          
          (*JNIenv)->CallVoidMethod(JNIenv, jcallbackCircuitChange, jcallbackCircuitChange_runID, jcv);
          break;
        case AM_DEPARTURE:
          printf("AM_DEPARTURE\n");
          fillCv(&cv, ((payloadArrivalDeparture*) (mp->payload))->circuit);
          cv.cv_departed = ((payloadArrivalDeparture*) (mp->payload))->ad;
            //(*theCallbackCircuitChange)(&cv);
            //mp->payload wich is char[]
          /* Set CircuitView */
          (*JNIenv)->SetIntField(JNIenv, jcv, jcv_nmembID, cv.cv_nmemb); 
          (*JNIenv)->SetIntField(JNIenv, jcv, jcv_membersID, cv.cv_members[MAX_MEMB]); 
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
