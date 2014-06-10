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

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include "interface.h"
#include "management_addr.h"
#include "iomsg.h"
#include "stateMachine.h"
#include "errorTrains.h"

#ifdef JNI
#include "counter.h"
#include "trains_Interface.h"
#include "jniContext.h"
#endif /* JNI */

sem_t *sem_init_done;

int trErrno;

#ifdef JNI

/* JNI global variables */
char *theJNICallbackCircuitChange;
char *theJNICallbackUtoDeliver;
JavaVM *jvm;

/* Global variables for caching methods and fields IDs
 * to speed up performances. 
 */

/* Callback IDs */
jmethodID jcircuitChangeID;
jmethodID jutoDeliverID;
 
/* Objects & Fields IDs */
jobject jmsghdr = NULL;
jfieldID jmsghdr_lenID = NULL;
jfieldID jmsghdr_typeID = NULL;

jobject jmsg = NULL;
jfieldID jmsg_hdrID = NULL;
jfieldID jmsg_payloadID = NULL;

jobject jcv = NULL;
jfieldID jcv_nmembID = NULL;
jfieldID jcv_joinedID = NULL;
jfieldID jcv_departedID = NULL;
jmethodID jcv_setMembersAddressID;

#endif /* JNI */

/**
 * @brief Initialization of trains protocol middleware

 * @param[in] trainsNumber The number of trains on the circuit
 * @param[in] wagonLength The length of the wagons in the trains
 * @param[in] waitNb The number of time to wait
 * @param[in] waitTime The time to wait (in microsecond)
 * @param[in] callbackCircuitChange Function to be called when there is a circuit changed (Arrival or departure of a process)
 * @param[in] callbackUtoDeliver    Function to be called when a message can be uto-delivered by trains protocol
 * @return 0 upon successful completion, or -1 if an error occurred (in which case, @a trErrno is set appropriately)
 */
#ifndef JNI
int trInit(int trainsNumber, int wagonLength, int waitNb, int waitTime,
    CallbackCircuitChange callbackCircuitChange,
    CallbackUtoDeliver callbackUtoDeliver){
#else /* JNI */
JNIEXPORT jint JNICALL Java_trains_Interface_trInit(JNIEnv *env, 
    jobject obj, jint trainsNumber, jint wagonLength, jint waitNB, 
    jint waitTime, 
    jstring callbackCircuitChange,
    jstring callbackUtoDeliver){
#endif /* JNI */
  int rc;
  pthread_t thread;
  char trainsHost[1024];
  char *trainsPort;
  int rank;
  char sem_name[128];
 
#ifdef JNI
  /* Converts Java strings to C strings*/
  char *myCallbackCircuitChange;
  char *myCallbackUtoDeliver;

  myCallbackCircuitChange = malloc(128*sizeof(char));
  myCallbackUtoDeliver = malloc(128*sizeof(char));
  
  const char *str = (*env)->GetStringUTFChars(env, callbackCircuitChange, 0);
  strncpy(myCallbackCircuitChange, str, 128);
  (*env)->ReleaseStringUTFChars(env, callbackCircuitChange, str);
  
  str = (*env)->GetStringUTFChars(env, callbackUtoDeliver, 0);
  strncpy(myCallbackUtoDeliver, str, 128);
  (*env)->ReleaseStringUTFChars(env, callbackUtoDeliver, str);
#endif /* JNI */
 
  if (trainsNumber > 0)
  ntr = trainsNumber;

  if (wagonLength > 0)
    wagonMaxLen = wagonLength;

  if(waitNb > 0)
    waitNbMax = waitNb;

  if (waitTime > 0)
    waitDefaultTime = waitTime;


  sprintf(sem_name, "sem_init_done_%d", getpid());
  sem_init_done = sem_open(sem_name, O_CREAT, 0600, 0);
#ifndef WINDOWS
  if(sem_init_done == SEM_FAILED)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "sem_open");
#else
  if(sem_init_done == NULL)
    ERROR_AT_LINE(EXIT_FAILURE, GetLastError(), __FILE__, __LINE__, "sem_open");
#endif
  pthread_mutex_init(&mutexWagonToSend, NULL );

  rc= pthread_cond_init(&condWagonToSend, NULL);
  assert(rc == 0);

#ifndef JNI
  theCallbackCircuitChange = callbackCircuitChange;
  theCallbackUtoDeliver = callbackUtoDeliver;
#else /* JNI */
  /* Get and format the name of the callback classes */
  theJNICallbackCircuitChange = malloc(255*sizeof(char));
  theJNICallbackUtoDeliver = malloc(255*sizeof(char));

  theJNICallbackCircuitChange = myCallbackCircuitChange;
  theJNICallbackUtoDeliver = myCallbackUtoDeliver;   
  format_class_name(theJNICallbackCircuitChange); 
  format_class_name(theJNICallbackUtoDeliver); 

  /* Store the JVM in the jvm variable so that 
   * we can perform the callbacks in the same JVM
   * in applicationMessage.c (in diferrent threads) */
  (*env)->GetJavaVM(env, &jvm);
#endif /* JNI */

  globalAddrArray = addrGenerator(LOCALISATION, NP);

  rc = gethostname(trainsHost,1024);
  if(rc != 0){
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__,
        "Error getting hostname");
  }
  trainsPort = getenv("TRAINS_PORT");
  if (trainsPort == NULL)
    ERROR_AT_LINE(EXIT_FAILURE,0,__FILE__,__LINE__,"TRAINS_PORT environment variable is not defined");
  rank = addrID(trainsHost,trainsPort,globalAddrArray);
  if (rank < 0)
    ERROR_AT_LINE(EXIT_FAILURE,0,__FILE__,__LINE__,"Could not find a line in %s file corresponding to TRAINS_HOST environment variable value (%s) and TRAINS_PORT environment variable value (%s)", LOCALISATION, trainsHost, trainsPort);
  myAddress=rankToAddr(rank);

  automatonInit();
  do {
    rc = sem_wait(sem_init_done);
  } while ((rc < 0) && (errno == EINTR));
  if (rc)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "sem_wait()");

  rc = pthread_create(&thread, NULL, utoDeliveries, NULL);
  if (rc)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  rc = pthread_detach(thread);
  if (rc)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");

  return 0;
}

/**
 * @brief TrainsProtocol version of ERROR_AT_LINE
 * @param[in] status
 * @param[in] errnum
 * @param[in] filename
 * @param[in] linenum
 * @param[in] format
 * @return void
 */
#ifndef JNI
void trError_at_line(int status, int errnum, const char *filename, unsigned int linenum, const char *format){
#else /* JNI */
JNIEXPORT void JNICALL Java_trains_Interface_trError_1at_1line
  (JNIEnv *env, jobject obj, jint status, jint errnu){
#endif /* JNI */
  fflush(stdout);
  fprintf(stderr, "basic version of trError_at_line\n");
}

/**
 * @brief TrainsProtocol version of perror
 * @param[in] errnum
 * @return void
 */
#ifndef JNI
void trPerror(int errnum){
#else /* JNI */
JNIEXPORT void JNICALL Java_trains_Interface_trPerror
  (JNIEnv *env, jobject obj, jint errnum){
#endif /* JNI */
  fprintf(stderr, "basic version of trPerror");
}

#ifndef JNI
int trTerminate(){
#else /* JNI */
JNIEXPORT jint JNICALL Java_trains_Interface_trTerminate
  (JNIEnv *env, jobject obj){
#endif /* JNI */
  return 0;
}

#ifdef JNI
void format_class_name(char *arg){
  int j;

  for (j = 0; j < strlen(arg) ; j++) {
    if (arg[j] == '.'){
      arg[j] = '/';
    }
  }
}

/* Caching the method IDs for the MessageHeader object */
JNIEXPORT void JNICALL Java_trains_Interface_initIDsMessageHeader(JNIEnv *env, jclass cls){
  jmethodID mid;
  jobject jobj;
 
  /* Instantiate a MessageHeader object */
  jclass class = (*env)->FindClass(env, "trains/MessageHeader"); 
  if (class == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find class MessageHeader");
  }

  mid = (*env)->GetMethodID(env, class,
                               "<init>", "(II)V");
  if (mid == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find constructor for MessageHeader");
  } 
     
  jobj = (*env)->NewObject(env, class, mid, 42, 0);
  if (jobj == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "instantiate jmsghdr");
  } 
    
  jmsghdr = (*env)->NewGlobalRef(env, jobj);
  if(jmsghdr == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "Global ref for CircuitView");
  }
  (*env)->DeleteLocalRef(env, jobj);

  jmsghdr_lenID = (*env)->GetFieldID(env, class, "len", "I");
  if (jmsghdr_lenID == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "get MessageHeader len field ID");
  }

  jmsghdr_typeID = (*env)->GetFieldID(env, class, "type", "C");
  if (jmsghdr_lenID == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "get MessageHeader type field ID");
  }
}

/* Caching the method IDs for the Message object */
JNIEXPORT void JNICALL Java_trains_Interface_initIDsMessage(JNIEnv *env, jclass cls){
  jmethodID mid;
  jobject jobj;
  
  /* Instantiate a Message object */
  jclass class = (*env)->FindClass(env, "trains/Message"); 
  if (class == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find class Message");
  }
 
  mid = (*env)->GetMethodID(env, class,
                               "<init>", "(Ltrains/MessageHeader;[B)V");
  if (mid == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find constructor for Message");
  } 
      
  jobj = (*env)->NewObject(env, class, mid, NULL, "");
  if (jobj == NULL){ 
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "instantiate jmsg");
  } 
        
  jmsg = (*env)->NewGlobalRef(env, jobj);
  if(jmsg == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "Global ref for CircuitView");
  }
  (*env)->DeleteLocalRef(env, jobj);

  jmsg_payloadID = (*env)->GetFieldID(env, class, "payload", "[B");
  if (jmsg_payloadID == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "get Message payload field ID");
  }

  jmsg_hdrID = (*env)->GetFieldID(env, class, "messageHeader", "Ltrains/MessageHeader;");
  if (jmsg_hdrID == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "get Message messageHeader field ID");
  }
}

/* Caching the method IDs for the CircuitView singleton */
JNIEXPORT void JNICALL Java_trains_Interface_initIDsCircuitView(JNIEnv *env, jclass cls){
  jmethodID mid;
  jobject jobj;
  
  /* Get the CircuitView singleton */
  jclass class = (*env)->FindClass(env, "trains/CircuitView"); 
  if (class == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find class CircuitView");
  }

  mid = (*env)->GetStaticMethodID(env, class, "getInstance", "()Ltrains/CircuitView;");
  if (mid == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find static factory for CircuitView");
  }  

  jobj = (*env)->CallStaticObjectMethod(env, class, mid);
  if (jobj == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "call static factory for CircuitView");
  }
  jcv = (*env)->NewGlobalRef(env, jobj);
  if(jcv == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "Global ref for CircuitView");
  }
  (*env)->DeleteLocalRef(env, jobj);

  jcv_nmembID = (*env)->GetFieldID(env, class, "nmemb", "I");
  if (jcv_nmembID == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "get CircuitView nmemb field ID");
  }

  jcv_joinedID = (*env)->GetFieldID(env, class, "joined", "I");
  if (jcv_joinedID == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "get CircuitView joined field ID");
  }

  jcv_departedID = (*env)->GetFieldID(env, class, "departed", "I");
  if (jcv_departedID == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "get CircuitView departed field ID");
  }

  jcv_setMembersAddressID = (*env)->GetMethodID(env, class, "setMembersAddress", "(II)V");
  if (mid == NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "find setMembersAddress methodID");
  }  
}

JNIEXPORT void JNICALL Java_trains_Interface_dumpCountersData(JNIEnv *env, jobject jobj, jbyteArray data){ 
  //jbyteArray counters_data_temp;
  //jbyteArray counters_data;

  int size;
  size = sizeof(counters);

  /*counters_data_temp = (env*)->NewByteArray(env, size);
  if(counters_data_temp = NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "create counters_data_temp byte array");
  }
  counters_data = (env*)->NewGlobalRef(env, counters_data_temp);
  if(counters_data = NULL){
    ERROR_AT_LINE(EXIT_FAILURE, 1, __FILE__, __LINE__, "create countersdata byte array");
  }
  (*env)->DeleteLocalRef(env, counters_data_temp);
*/
  (*env)->SetByteArrayRegion(env, data, 0, size, (jbyte *) (&counters));
}


JNIEXPORT jint JNICALL Java_trains_Interface_getMAX_1MEMB(JNIEnv *env, jclass cls){
  return MAX_MEMB;
}

JNIEXPORT jint JNICALL Java_trains_Interface_getMyAddress(JNIEnv *env, jclass cls){
  return myAddress;
}
#endif /* JNI */
