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
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

#include "interface.h"
#include "management_addr.h"
#include "iomsg.h"
#include "stateMachine.h"
#include <string.h>

#include "applicationMessage.h"
#include "trains_Interface.h"
#include "errorTrains.h"

char *theJNICallbackCircuitChange;
char *theJNICallbackUtoDeliver;
JavaVM *jvm;

sem_t sem_init_done;

int trErrno;

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
JNIEXPORT jint JNICALL Java_trains_Interface_trInit(JNIEnv *env, 
    jobject obj, jint trainsNumber, jint wagonLength, jint waitNB, 
    jint waitTime, 
    jstring callbackCircuitChange,
    jstring callbackUtoDeliver){
    //jobject callbackCircuitChange, 
    //jobject callbackUtoDeliver){
  
  int rc;
  pthread_t thread;
  char trainsHost[1024];
  char *trainsPort;
  int rank;
 
  /* Converts Java strings to C strings*/
  char *myCallbackCircuitChange;
  char *myCallbackUtoDeliver;

  myCallbackCircuitChange = malloc(128*sizeof(char));
  myCallbackUtoDeliver = malloc(128*sizeof(char));
  
  const char *str = (*env)->GetStringUTFChars(env, callbackCircuitChange, 0);
  //XXX - length should be enough
  strncpy(myCallbackCircuitChange, str, 128);
  (*env)->ReleaseStringUTFChars(env, callbackCircuitChange, str);
  
  str = (*env)->GetStringUTFChars(env, callbackUtoDeliver, 0);
  //XXX - length should be enough
  strncpy(myCallbackUtoDeliver, str, 128);
  (*env)->ReleaseStringUTFChars(env, callbackUtoDeliver, str);

  if (trainsNumber > 0)
  ntr = trainsNumber;

  if (wagonLength > 0)
    wagonMaxLen = wagonLength;

  if(waitNb > 0)
    waitNbMax = waitNb;

  if (waitTime > 0)
    waitDefaultTime = waitTime;


  rc = sem_init(&sem_init_done,0,0);
  if(rc)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "sem_init");

  pthread_mutex_init(&mutexWagonToSend, NULL );

  rc= pthread_cond_init(&condWagonToSend, NULL);
  assert(rc == 0);

  theJNICallbackCircuitChange = malloc(128*sizeof(char));
  theJNICallbackUtoDeliver = malloc(128*sizeof(char));

  theJNICallbackCircuitChange = myCallbackCircuitChange;
  theJNICallbackUtoDeliver = myCallbackUtoDeliver;
  (*env)->GetJavaVM(env, &jvm);

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
    rc = sem_wait(&sem_init_done);
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
JNIEXPORT void JNICALL Java_trains_Interface_trError_1at_1line
  (JNIEnv *env, jobject obj, jint status, jint errnu){
  fflush(stdout);
  fprintf(stderr, "basic version of trError_at_line\n");
}

/**
 * @brief TrainsProtocol version of perror
 * @param[in] errnum
 * @return void
 */
JNIEXPORT void JNICALL Java_trains_Interface_trPerror
  (JNIEnv *env, jobject obj, jint errnum){
  fprintf(stderr, "basic version of trPerror");
}

JNIEXPORT jint JNICALL Java_trains_Interface_trTerminate
  (JNIEnv *env, jobject obj){
  return 0;
}
