/*
 Mock module designed to run the basic test while middleware is not yet ready
 */

#include <stdio.h>
#include "trains.h"

address my_address = 0x01;
CallbackUtoDeliver lud;
int tr_errno = 0;

char *addr_2_str(char *s, address ad){
  sprintf(s, "0x%02x", ad);
  return s;
}

message *newmsg(int payloadSize){
  static char byteArray[256]; /* 256 to be sure we will never have problems to store this message */
  message *mp = (message*)byteArray;
  mp->len = sizeof(mp->len)+payloadSize;
  return mp;
}

void tr_error_at_line(int status, int errnum, const char *filename, unsigned int linenum, const char *format){
  fflush(stdout);
  fprintf(stderr, "basic version of tr_error_at_line\n");
}

int tr_init(CallbackCircuitChange aCallbackCircuitChange, CallbackUtoDeliver aCallbackUtoDeliver){
  circuitview cv;
  cv.cv_nmemb = 1;
  cv.cv_members[0] = my_address;
  cv.cv_joined = my_address;
  cv.cv_departed = 0;
  (*aCallbackCircuitChange)(&cv);
  lud = aCallbackUtoDeliver;
  return 0;
}

void tr_perror(int errnum){
  fprintf(stderr, "basic version of tr_perror");
}

int uto_broadcast(message *mp){
  (*lud)(my_address,mp);
  return 0;
}

int tr_terminate(){
  return 0;
}
