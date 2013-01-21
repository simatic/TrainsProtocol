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

/*
 Program for doing performance tests
 Syntax:
 perf --help

 For the printf, we have used ";" as the separator between data.
 As explained in http://forthescience.org/blog/2009/04/16/change-separator-in-gnuplot/, to change
 the default separator " " in gnuplot, do:
 set datafile separator ";"
 */
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include "trains.h"
#include "counter.h"
#include "errorTrains.h"
#include "perf_InterfaceJNI.h"

struct rusage rusageBegin;
struct rusage rusageEnd;
t_counters countersBegin;
t_counters countersEnd;

int getDiffTimeval(struct timeval stop, struct timeval start){
  struct timeval diffTimeval;
  timersub(&stop, &start, &diffTimeval);
  return (int) (diffTimeval.tv_sec * 1000000 + diffTimeval.tv_usec);
}

JNIEXPORT void JNICALL Java_perf_InterfaceJNI_getrusageBegin(JNIEnv *env, jobject jobj){
  if (getrusage(RUSAGE_SELF, &rusageBegin) < 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "getrusageBegin");
}

JNIEXPORT void JNICALL Java_perf_InterfaceJNI_getrusageEnd(JNIEnv *env, jobject jobj){
  if (getrusage(RUSAGE_SELF, &rusageEnd) < 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "getrusageBegin");
}

JNIEXPORT void JNICALL Java_perf_InterfaceJNI_setcountersBegin(JNIEnv *env, jobject jobj, jbyteArray data){
  //memcpy(&countersBegin, data, (*env)->GetArrayLength(env, data));
  int size = (*env)->GetArrayLength(env, data);
  char *buf = malloc(sizeof(t_counter)*18);
  (*env)->GetByteArrayRegion(env, data, 0, sizeof(t_counter)*18, (jbyte *)buf);

  memcpy(&countersBegin, buf, sizeof(t_counter)*18);
}

JNIEXPORT void JNICALL Java_perf_InterfaceJNI_setcountersEnd(JNIEnv *env, jobject jobj, jbyteArray data){
  //memcpy(&countersEnd, data, (*env)->GetArrayLength(env, data));
  //countersEnd =  data;
  int size = (*env)->GetArrayLength(env, data);
  char *buf = malloc(sizeof(t_counter)*18);
  (*env)->GetByteArrayRegion(env, data, 0, sizeof(t_counter)*18, (jbyte *)buf);

  memcpy(&countersEnd, buf, sizeof(t_counter)*18);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getru_1utime(JNIEnv *env, jobject jobj){
  return getDiffTimeval(rusageEnd.ru_utime, rusageBegin.ru_utime);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getru_1stime(JNIEnv *env, jobject jobj){
  return getDiffTimeval(rusageEnd.ru_stime, rusageBegin.ru_stime);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getmessages_1delivered(JNIEnv *env, jobject jobj){
  return (countersEnd.messages_delivered -  countersBegin.messages_delivered);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getmessages_1bytes_1delivered(JNIEnv *env, jobject jobj){
  return (countersEnd.messages_bytes_delivered -  countersBegin.messages_bytes_delivered);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_gettrains_1bytes_1received(JNIEnv *env, jobject jobj){
  return (countersEnd.trains_bytes_received -  countersBegin.trains_bytes_received);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_gettrains_1received(JNIEnv *env, jobject jobj){
  return (countersEnd.trains_received -  countersBegin.trains_received);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getrecent_1trains_1received(JNIEnv *env, jobject jobj){
  return (countersEnd.recent_trains_received -  countersBegin.recent_trains_received);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getrecent_1trains_1bytes_1received(JNIEnv *env, jobject jobj){
  return (countersEnd.recent_trains_bytes_received -  countersBegin.recent_trains_bytes_received);
}  

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getwagons_1delivered(JNIEnv *env, jobject jobj){
  return (countersEnd.wagons_delivered -  countersBegin.wagons_delivered);
}
JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getwait_1states(JNIEnv *env, jobject jobj){
  return (countersEnd.wait_states - countersBegin.wait_states);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getcomm_1read(JNIEnv *env, jobject jobj){
  return (countersEnd.comm_read - countersBegin.comm_read);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getcomm_1read_1bytes(JNIEnv *env, jobject jobj){
  return (countersEnd.comm_read_bytes - countersBegin.comm_read_bytes);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getcomm_1readFully(JNIEnv *env, jobject jobj){
  return (countersEnd.comm_readFully - countersBegin.comm_readFully);
}  

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getcomm_1readFully_1bytes(JNIEnv *env, jobject jobj){
  return (countersEnd.comm_readFully_bytes - countersBegin.comm_readFully_bytes);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getcomm_1write(JNIEnv *env, jobject jobj){
  return (countersEnd.comm_write - countersBegin.comm_write);
}  

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getcomm_1write_1bytes(JNIEnv *env, jobject jobj){
  return (countersEnd.comm_write_bytes - countersBegin.comm_write_bytes);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getcomm_1writev(JNIEnv *env, jobject jobj){
  return (countersEnd.comm_writev - countersBegin.comm_writev);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getcomm_1writev_1bytes(JNIEnv *env, jobject jobj){
  return (countersEnd.comm_writev_bytes - countersBegin.comm_writev_bytes);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getnewmsg(JNIEnv *env, jobject jobj){
  return (countersEnd.newmsg - countersBegin.newmsg);
}

JNIEXPORT jint JNICALL Java_perf_InterfaceJNI_getflowControl(JNIEnv *env, jobject jobj){
  return (countersEnd.flowControl - countersBegin.flowControl);
}
