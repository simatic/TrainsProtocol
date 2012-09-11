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
 Program for doing  latency performance tests
 Syntax:
 latency --help

 For the printf, we have used ";" as the separator between data.
 As explained in http://forthescience.org/blog/2009/04/16/change-separator-in-gnuplot/, to change
 the default separator " " in gnuplot, do:
 set datafile separator ";"
 */
#include <stdlib.h>
#include <stdio.h>

#ifdef LATENCY_TEST

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <getopt.h>
#include <error.h>
#include <errno.h>
#include <semaphore.h>
#include <strings.h>
#include <pthread.h>
#include <string.h>
#include "trains.h"
#include "counter.h"
#include "param.h"
#include "latencyData.h"

/* Semaphore used to block main thread until there are enough participants */
static sem_t semWaitEnoughMembers;

/* Rank of the process in the group of participants when there are enough
 participants. */
static int rank;

/* Boolean indicating that the measurement phase is over */
static bool measurementDone = false;

/* Boolean indicating that the measurement phase is processing */
bool measurementPhase = false;

/* Storage to measure execution time of trInit */
struct timeval timeTrInitBegin, timeTrInitEnd;

/* Global variables of the program */
pingRecord record;
address pingResponder;
address pingSender;
struct timeval sendTime, sendDate, receiveDate, latency;
int pingMessageSize = sizeof(address) + sizeof(struct timeval);

/* Storage of the program name, which we'll use in error messages.  */
char *programName;

/* Parameters of the program */
int broadcasters           = -1;
int cooldown               = 10; /* Default value = 10 seconds */
int frequencyOfPing  = 10000; /* Default value = 10 000 */
int alternateMaxWagonLen   = (1<<15); /* Default value 32768 */
int measurement            = 600; /* Default value = 600 seconds */
int number                 = -1;
int size                   = -1;
int trainsNumber           = -1;
bool verbose               = false; /* Default value = limited display */
int warmup                 = 300; /* Default value = 300 seconds */

/* Description of long options for getopt_long.  */
static const struct option longOptions[] = {
    { "broadcasters",     1, NULL, 'b' },
    { "cooldown",         1, NULL, 'c' },
    { "frequencyOfPing",  1, NULL, 'f' },
    { "help",             0, NULL, 'h' },
    { "wagonMaxLen",      1, NULL, 'l' },
    { "measurement",      1, NULL, 'm' },
    { "number",           1, NULL, 'n' },
    { "size",             1, NULL, 's' },
    { "trainsNumber",     1, NULL, 't' },
    { "verbose",          0, NULL, 'v' },
    { "warmup",           1, NULL, 'w' },
    { NULL,               0, NULL, 0   }
};

/* Description of short options for getopt_long.  */
static const char* const shortOptions = "b:c:f:hl:m:n:s:t:vw:";

/* Usage summary text.  */
static const char* const usageTemplate =
    "Usage: %s [ options ]\n"
        "  -b, --broadcasters number       Number of broadcasting processes.\n"
        "  -c, --cooldown seconds          Duration of cool-down phase (default = 10).\n"
        "  -f, --frequencyPing             Frequency of AM_PING messages (default = 10000)\n"
        "  -h, --help                      Print this information.\n"
        "  -l, --wagonMaxLen               Maximum length of wagons (default = 32768)\n"
        "  -m, --measurement seconds       Duration of measurement phase (default = 600).\n"
        "  -n, --number                    Number of participating processes.\n"
        "  -s, --size bytes                Bytes contained in each application message uto-broadcasted.\n"
        "  -t, --trainsNumber              Number of trains which should be used by the protocol.\n"
        "  -v, --verbose                   Print verbose messages.\n"
        ""
        "  -w, --warmup seconds            Duration of warm-up phase (default = 300).\n";

/* Print usage information and exit.  If IS_ERROR is non-zero, write to
 stderr and use an error exit code.  Otherwise, write to stdout and
 use a non-error termination code.  Does not return.
 */
static void printUsage(int is_error){
  fprintf(is_error ? stderr : stdout, usageTemplate, programName);
  exit(is_error ? EXIT_FAILURE : EXIT_SUCCESS);
}

/* Converts the value stored in optarg into an integer.
 Calls printUsage is conversion is not possible or if the
 value is incorrect.
 */
int optArgToCorrectValue(){
  long value;
  char* end;

  value = strtol(optarg, &end, 10);
  if (*end != '\0')
    /* The user specified non-digits for this number.  */
    printUsage(EXIT_FAILURE);
  if (value <= 0)
    /* The user gave an incorrect value. */
    printUsage(EXIT_FAILURE);
  return value;
}

/* Checks that a parameter (which name is pointed by name) has been given a value (its value is no more negative) */
void check(int value, char *name){
  if (value < 0) {
    fprintf(stderr, "\"%s\" must be specified\n", name);
    printUsage(EXIT_FAILURE);
  }
}

/* Prints message msg, followed by a ";" and the difference between stop and start */
void printDiffTimeval(char *msg, struct timeval stop, struct timeval start){
  struct timeval diffTimeval;
  timersub(&stop, &start, &diffTimeval);
  printf("%s ; %d.%06d\n", msg, (int) diffTimeval.tv_sec,
      (int) diffTimeval.tv_usec);
}

/* Callback for circuit changes */
void callbackCircuitChange(circuitView *cp){
  char s[MAX_LEN_ADDRESS_AS_STR];

  printf("!!! ******** callbackCircuitChange called with %d members (process ",
      cp->cv_nmemb);
  if (!addrIsNull(cp->cv_joined)) {
    printf("%s has arrived)\n", addrToStr(s, cp->cv_joined));
  } else {
    printf("%s is gone)\n", addrToStr(s, cp->cv_departed));
    if (!measurementDone) {
      printf("!!! ******** Experience has failed ******** !!!\n");
      exit(EXIT_FAILURE);
    }
  }

  if (cp->cv_nmemb >= number) {
    // We compute the rank of the process in the group
    for (rank = 0; (rank < cp->cv_nmemb) && !addrIsMine(cp->cv_members[rank]);
        rank++)
      ;
    // We can start the experience
    printf("!!! ******** enough members to start utoBroadcasting\n");

    // The participants choose the pingResponder : the participant which will respond to the AM_PING messages
    pingResponder = cp->cv_members[0];

    printf("!!! The pingResponder for this experience is %d:%s:%s\n",
        addrToRank(pingResponder), addrToHostname(pingResponder), addrToPort(pingResponder));

    // The experience starts
    int rc = sem_post(&semWaitEnoughMembers);
    if (rc)
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "sem_post()");
  }
}

/* Callback for messages to be UTO-delivered */
void callbackUtoDeliver(address sender, message *mp){
  char s[MAX_LEN_ADDRESS_AS_STR];
  static int nbRecMsg = 0;

  if (payloadSize(mp) != size) {
    fprintf(stderr,
        "Error in file %s:%d : Payload size is incorrect: it is %lu when it should be %d\n",
        __FILE__, __LINE__, payloadSize(mp), size);
    exit(EXIT_FAILURE);
  }

  if (mp->header.typ == AM_PING) {
    if (myAddress == pingResponder) {

      message *pongMsg = newmsg(size);
      pongMsg->header.typ = AM_PONG;
      memcpy(pongMsg->payload, mp->payload, pingMessageSize);

      int rc;
      if ((rc = utoBroadcast(pongMsg)) < 0) {
        trError_at_line(rc, trErrno, __FILE__, __LINE__, "utoBroadcast()");
        exit(EXIT_FAILURE);
      }
    }
  } else if (mp->header.typ == AM_PONG) {

    memcpy(&pingSender, mp->payload, sizeof(address));
    if (pingSender == myAddress) {
      memcpy(&sendDate, mp->payload + sizeof(address), sizeof(struct timeval));
      gettimeofday(&receiveDate, NULL );
      timersub(&receiveDate, &sendDate, &latency);

      if (measurementPhase) {
        recordValue(latency, &record);
      }

    }
  }

  nbRecMsg++;

  if (verbose)
    printf("!!! %5d-ieme message (recu de %s / contenu = %5d)\n", nbRecMsg,
        addrToStr(s, sender), *((int*) (mp->payload)));

}

/* Thread taking care of respecting the times of the experiment. */
void *timeKeeper(void *null){
  int rc;
  t_counters countersBegin, countersEnd;
  struct rusage rusageBegin, rusageEnd;
  struct timeval timeBegin, timeEnd;
  struct timeval startSomme, stopSomme, diffCPU, diffTimeval;

  measurementPhase = false;
  // Warm-up phase
  usleep(warmup * 1000000);

  // Measurement phase
  if (gettimeofday(&timeBegin, NULL ) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "gettimeofday");
  if (getrusage(RUSAGE_SELF, &rusageBegin) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "getrusage");
  countersBegin = counters;

  measurementPhase = true;
  usleep(measurement * 1000000);

  if (gettimeofday(&timeEnd, NULL ) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "gettimeofday");
  if (getrusage(RUSAGE_SELF, &rusageEnd) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "getrusage");
  countersEnd = counters;

  measurementPhase = false;
  measurementDone = true;
  // Cool-down phase
  usleep(cooldown * 1000000);

  // We display the results
  printf(
      "%s --broadcasters %d --cooldown %d --frequencyPing %d --wagonMaxLen %d --measurement %d --number %d --size %d --trainsNumber %d  --warmup %d\n",
      programName, broadcasters, cooldown, frequencyOfPing, wagonMaxLen, measurement, number, size,
      trainsNumber, warmup);

  printDiffTimeval("time for tr_init (in sec)", timeTrInitEnd, timeTrInitBegin);

  printDiffTimeval("elapsed time (in sec)", timeEnd, timeBegin);

  printDiffTimeval("ru_utime (in sec)", rusageEnd.ru_utime,
      rusageBegin.ru_utime);
  printDiffTimeval("ru_stime (in sec)", rusageEnd.ru_stime,
      rusageBegin.ru_stime);

  timeradd(&rusageBegin.ru_utime, &rusageBegin.ru_stime, &startSomme);
  timeradd(&rusageEnd.ru_utime, &rusageEnd.ru_stime, &stopSomme);
  printDiffTimeval("ru_utime+ru_stime (in sec)", stopSomme, startSomme);

  printf("number of messages delivered to the application ; %llu\n", countersEnd.messages_delivered - countersBegin.messages_delivered);
  printf("number of bytes delivered to the application ; %llu\n", countersEnd.messages_bytes_delivered - countersBegin.messages_bytes_delivered);
  printf("number of bytes of trains received from the network ; %llu\n", countersEnd.trains_bytes_received - countersBegin.trains_bytes_received);
  printf("number of trains received from the network ; %llu\n", countersEnd.trains_received - countersBegin.trains_received);
  printf("number of bytes of recent trains received from the network ; %llu\n", countersEnd.recent_trains_bytes_received - countersBegin.recent_trains_bytes_received);
  printf("number of recent trains received from the network ; %llu\n", countersEnd.recent_trains_received - countersBegin.recent_trains_received);
  printf("number of wagons delivered to the application ; %llu\n", countersEnd.wagons_delivered - countersBegin.wagons_delivered);
  printf("number of times automaton has been in state WAIT ; %llu\n", countersEnd.wait_states - countersBegin.wait_states);
  printf("number of calls to commRead() ; %llu\n", countersEnd.comm_read - countersBegin.comm_read);
  printf("number of bytes read by commRead() calls ; %llu\n", countersEnd.comm_read_bytes - countersBegin.comm_read_bytes);
  printf("number of calls to commReadFully() ; %llu\n", countersEnd.comm_readFully - countersBegin.comm_readFully);
  printf("number of bytes read by commReadFully() calls ; %llu\n", countersEnd.comm_readFully_bytes - countersBegin.comm_readFully_bytes);
  printf("number of calls to commWrite() ; %llu\n", countersEnd.comm_write - countersBegin.comm_write);
  printf("number of bytes written by commWrite() calls ; %llu\n", countersEnd.comm_write_bytes - countersBegin.comm_write_bytes);
  printf("number of calls to commWritev() ; %llu\n", countersEnd.comm_writev - countersBegin.comm_writev);
  printf("number of bytes written by commWritev() calls ; %llu\n", countersEnd.comm_writev_bytes - countersBegin.comm_writev_bytes);
  printf("number of calls to newmsg() ; %llu\n", countersEnd.newmsg - countersBegin.newmsg);
  printf("number of times there was flow control when calling newmsg() ; %llu\n", countersEnd.flowControl - countersBegin.flowControl);

  timersub(&stopSomme, &startSomme, &diffCPU);
  timersub(&timeEnd, &timeBegin, &diffTimeval);
  printf(
      "Broadcasters / number / size / ntr / Average number of delivered wagons per recent train received / Average number of msg per wagon / Throughput of uto-broadcasts in Mbps ; %d ; %d ; %d ; %d ; %g ; %g ; %g ; %g\n",
      broadcasters, number, size, ntr,
      ((double) (countersEnd.wagons_delivered - countersBegin.wagons_delivered))
          / ((double) (countersEnd.recent_trains_received
              - countersBegin.recent_trains_received)),
      ((double) (countersEnd.messages_delivered
          - countersBegin.messages_delivered))
          / ((double) (countersEnd.wagons_delivered
              - countersBegin.wagons_delivered)),
      ((double) (countersEnd.messages_bytes_delivered
          - countersBegin.messages_bytes_delivered) * 8)
          / ((double) (diffTimeval.tv_sec * 1000000 + diffTimeval.tv_usec)),
      ((double) (diffCPU.tv_sec * 1000000 + diffCPU.tv_usec)
          / (double) (diffTimeval.tv_sec * 1000000 + diffTimeval.tv_usec)));

  // Latency results

  setStatistics(&record);
  if (rank >= broadcasters) {
    printf("\nWARNING : This participant was not a broadcaster\n"
        "It didn't send any PING message\n");
  }
  printf("\n"
      "Number of ping records during this experience : %u\n"
      "Average latency  (ms)   : %.2lf\n"
      "Variance                : %lf\n"
      "Standard deviation      : %lf\n"
      "95%% confidence interval : [%.2lf ; %.2lf]\n"
      "99%% confidence interval : [%.2lf ; %.2lf]\n", record.currentRecordsNb,
      record.mean, record.variance, record.standardDeviation,
      record.min95confidenceInterval, record.max95confidenceInterval,
      record.min99confidenceInterval, record.max99confidenceInterval);

  // Termination phase
  freePingRecord(&record);
  rc = trTerminate();
  if (rc < 0) {
    trError_at_line(rc, trErrno, __FILE__, __LINE__, "tr_init()");
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);

  return NULL ;
}

void startTest(){
  int rc;
  int rankMessage = 0;
  pthread_t thread;
  int pingMessagesCounter = 0;
  record = newPingRecord();

  rc = sem_init(&semWaitEnoughMembers, 0, 0);
  if (rc)
    error_at_line(rc, errno, __FILE__, __LINE__, "sem_init()");

  // We initialize the trains protocol
  if (gettimeofday(&timeTrInitBegin, NULL ) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "gettimeofday");
  rc = trInit(trainsNumber, wagonMaxLen, 0, 0, callbackCircuitChange, callbackUtoDeliver);
  if (rc < 0) {
    trError_at_line(rc, trErrno, __FILE__, __LINE__, "tr_init()");
    exit(EXIT_FAILURE);
  }
  if (gettimeofday(&timeTrInitEnd, NULL ) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "gettimeofday");

  // We wait until there are enough members
  do {
    rc = sem_wait(&semWaitEnoughMembers);
  } while ((rc < 0) && (errno == EINTR));
  if (rc)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "sem_wait()");

  // We start the warm-up phase
  rc = pthread_create(&thread, NULL, timeKeeper, NULL );
  if (rc < 0)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  rc = pthread_detach(thread);
  if (rc < 0)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");

  // We check if process should be a broadcasting process
  if (rank < broadcasters) {
    // It is the case
    do {
      message *mp = NULL;
      if (pingMessagesCounter == 0) {
        mp = newmsg(size);
        if (mp == NULL ) {
          trError_at_line(rc, trErrno, __FILE__, __LINE__, "newPingMsg()");
          exit(EXIT_FAILURE);
        }
        rankMessage++;
        mp->header.typ = AM_PING;
        gettimeofday(&sendTime, NULL);
        memcpy(mp->payload, &myAddress, sizeof(address));
        memcpy((mp->payload) + sizeof(address), &sendTime, sizeof(struct timeval));

      } else {
        mp = newmsg(size);
        if (mp == NULL ) {
          trError_at_line(rc, trErrno, __FILE__, __LINE__, "newmsg()");
          exit(EXIT_FAILURE);
        }
        rankMessage++;
        *((int*) (mp->payload)) = rankMessage;
      }

      pingMessagesCounter = (pingMessagesCounter + 1) % frequencyOfPing;
      if ((rc = utoBroadcast(mp)) < 0) {
        trError_at_line(rc, trErrno, __FILE__, __LINE__, "utoBroadcast()");
        exit(EXIT_FAILURE);
      }

    } while (1);
  } else {
    printf("\nWARNING : this process is not a broadcaster\n"
        "It will not send PING messages during this experience\n"
        "Thus the ping results are not to be taken into account\n\n");
    while (1);
  }
}

#endif /* LATENCY_TEST */

int main(int argc, char *argv[]){
#ifdef LATENCY_TEST
  int next_option;

  /* Store the program name, which we'll use in error messages.  */
  programName = argv[0];



  /* Parse options.  */
  do {
    next_option = getopt_long(argc, argv, shortOptions, longOptions, NULL );
    switch (next_option) {
    case 'b':
      /* User specified -b or --broadcasters.  */
      broadcasters = optArgToCorrectValue();
      break;

    case 'c':
      /* User specified -c or --cooldown.  */
      cooldown = optArgToCorrectValue();
      break;

    case 'f':
      frequencyOfPing = optArgToCorrectValue();
      break;

    case 'h':
      /* User specified -h or --help.  */
      printUsage(EXIT_SUCCESS);
      break;

    case 'l':
      alternateMaxWagonLen = optArgToCorrectValue();
      break;

    case 'm':
      /* User specified -m or --measurement.  */
      measurement = optArgToCorrectValue();
      break;

    case 'n':
      /* User specified -n or --number.  */
      number = optArgToCorrectValue();
      break;

    case 's':
      /* User specified -s or --size.  */
      size = optArgToCorrectValue();
      break;

    case 't':
      /* User specified -t or --trainsNumber.  */
      trainsNumber = optArgToCorrectValue();
      break;

    case 'v':
      /* User specified -v or --verbose.  */
      verbose = true;
      break;

    case 'w':
      /* User specified -w or --warmup.  */
      warmup = optArgToCorrectValue();
      break;

    case '?':
      /* User specified an unrecognized option.  */
      printUsage(EXIT_FAILURE);
      break;

    case -1:
      /* Done with options.  */
      break;

    default:
      abort();
    }
  } while (next_option != -1);

  /* This program takes no additional arguments.  Issue an error if the
   user specified any.  */
  if (optind != argc)
    printUsage(1);

  /* Check that parameters without default values were specified. */
  check(broadcasters, "broadcasters");
  check(number, "number");
  check(size, "size");
  check(trainsNumber, "trainsNumber");

  if (size < pingMessageSize){
    fprintf(stderr, "Size too small ==> set to minimal value %d\n", pingMessageSize);
    size = pingMessageSize;
  }

  /* Initialize data external to this mudule */
  ntr = trainsNumber;
  wagonMaxLen = alternateMaxWagonLen;

  if (wagonMaxLen < size){
    fprintf(stderr, "wagonMaxLength too small ==> results will not be significative");
  }

  /* We can start the test */
  startTest();

#else /* LATENCY_TEST */
  printf("To run latency tests, you have to compile the library with the tests target\n");
#endif /* LATENCY_TEST */

  return EXIT_SUCCESS;
}

