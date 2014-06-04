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
 Program for doing wagon filling duration tests

 For the printf, we have used ";" as the separator between data.
 As explained in http://forthescience.org/blog/2009/04/16/change-separator-in-gnuplot/, to change
 the default separator " " in gnuplot, do:
 set datafile separator ";"
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "trains.h"
#include "stateMachine.h"
#include "interface.h"
#include "advanced_struct.h"
#include "connect.h"
#include "counter.h"

int main(int argc, char *argv[]){

  int i, j;
  int msgCounter;
  int wagonSizes[] = { 1024, 2048, 4096, 8192, 16384, 32768, 65536 };
  int payloadSizes[] = { 10, 100, 200, 500, 1000, 2000, 5000, 10000, 15000,
      20000 };
  struct timeval debut, fin, duree;
  struct rusage debutCPU, finCPU, dureeCPU;
  long elapsedTime, cpuTime;
  int rc;

  MUTEX_LOCK(stateMachineMutex);
  automatonState = SEVERAL;
  MUTEX_UNLOCK(stateMachineMutex);

  printf("Début de l'experience\n");

  for (i = 0; i < 7; i++) {
    for (j = 0; j < 10; j++) {
      // initialisation du wagon
      wagonMaxLen = wagonSizes[i];
      wagonToSend = newWiw();
      msgCounter = 0;

// start timers
      printf("Début du remplissage...\n");
      getrusage(RUSAGE_SELF, &debutCPU);
      gettimeofday(&debut, NULL );

// remplissage du wagon

      while (wagonToSend->p_wagon->header.len + payloadSizes[j]
          + sizeof(messageHeader) < wagonMaxLen) {
        message *mp = newmsg(payloadSizes[j]);
        if (mp == NULL ) {
	  rc = -1; //fill rc by hand
          trError_at_line(rc, trErrno, __FILE__, __LINE__, "newmsg()");
          exit(EXIT_FAILURE);
        }
	rc = utoBroadcast(mp);
        if (rc < 0) {
          trError_at_line(rc, trErrno, __FILE__, __LINE__, "utoBroadcast()");
          exit(EXIT_FAILURE);
        }
        msgCounter++;
      }

// stop timers
      gettimeofday(&fin, NULL );
      getrusage(RUSAGE_SELF, &finCPU);
      printf("Remplissage terminé\n");

// calculs
      timersub(&(finCPU.ru_utime), &(debutCPU.ru_utime), &(dureeCPU.ru_utime));
      timersub(&(finCPU.ru_stime), &(debutCPU.ru_stime), &(dureeCPU.ru_stime));
      timersub(&fin, &debut, &duree);

      elapsedTime = (1000000 * duree.tv_sec + duree.tv_usec);
      cpuTime = ((1000000
          * (dureeCPU.ru_utime.tv_sec + dureeCPU.ru_stime.tv_sec))
          + dureeCPU.ru_utime.tv_usec + dureeCPU.ru_stime.tv_usec);

      printf("********************************\n"
          "*********************************\n"
          "Messages de %d octets dans un wagon de %d octets\n", payloadSizes[j],
          wagonSizes[i]);
      printf("%d messages écrits\n", msgCounter);
      if (msgCounter > 0) {
        printf(
            "Temps absolu écoulé :          %9ld usec par message (%9ld au total)\n",
            elapsedTime / msgCounter, elapsedTime);
        printf(
            "Temps CPU (user+sys) écoulé :  %9ld usec par message (%9ld au total)\n",
            cpuTime / msgCounter, cpuTime);
      }
      printf("**************************\n");

      // free ressources
      freeWiw(wagonToSend);

    }
  }

  return EXIT_SUCCESS;
}
