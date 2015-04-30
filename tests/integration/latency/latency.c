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
 Program for doing latency performance tests
 Syntax:
 latency --help

 Principle:
 - All processes spend their time broadcasting PLAIN messages of size bytes
   (specified thanks to --size parameter).
 - When the test starts launched, the first process of the view broadcasts a 
   PING message containing the time at which the message is sent.
 - When a process receives a PING message, it broadcasts a PONG message
   containing the time which was contained in PING message.
 - When the process which initially sent the PING message has received the
   PONG message of all members of the view, it memorizes the delay between 
   its PING message and the last PONG message.
 - When a process p different from the process q which initially sent the PING
   message receives a PONG message from all members of the view, if process p
   is successor of process q in the view, it sends a PING message containing 
   the time at which the message is sent. 

 For the printf, we have used ";" as the separator between data.
 As explained in http://forthescience.org/blog/2009/04/16/change-separator-in-gnuplot/, to change
 the default separator " " in gnuplot, do:
 set datafile separator ";"
 */
#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <semaphore.h>
#include <strings.h>
#include <pthread.h>
#include "trains.h"
#include "counter.h"
#include "latencyData.h"
#include "errorTrains.h"

/* Type of messages broadcast */
#define PLAIN (FIRST_VALUE_AVAILABLE_FOR_MESS_TYP)
#define PING  (FIRST_VALUE_AVAILABLE_FOR_MESS_TYP + 1)
#define PONG  (FIRST_VALUE_AVAILABLE_FOR_MESS_TYP + 2)

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
address pingSender;
int minMessageSize = sizeof(int); // to store the integer of PLAIN messages
circuitView lastView;

/* Storage of the program name, which we'll use in error messages.  */
char *programName;

/* Parameters of the program */
int broadcasters           = -1;
int cooldown               = 10; /* Default value = 10 seconds */
int alternateMaxWagonLen   = (1<<15); /* Default value 32768 */
int measurement            = 600; /* Default value = 600 seconds */
int number                 = -1;
int size                   = -1;
int trainsNumber           = -1;
bool verbose               = false; /* Default value = limited display */
int warmup                 = 300; /* Default value = 300 seconds */
t_reqOrder reqOrder = UNIFORM_TOTAL_ORDER; /* default value = UNIFORM_TOTAL_ORDER */

/* Description of long options for getopt_long.  */
static const struct option longOptions[] = {
    { "broadcasters",     1, NULL, 'b' },
    { "cooldown",         1, NULL, 'c' },
    { "help",             0, NULL, 'h' },
    { "wagonMaxLen",      1, NULL, 'l' },
    { "measurement",      1, NULL, 'm' },
    { "number",           1, NULL, 'n' },
    { "reqOrder",         1, NULL, 'r' },
    { "size",             1, NULL, 's' },
    { "trainsNumber",     1, NULL, 't' },
    { "verbose",          0, NULL, 'v' },
    { "warmup",           1, NULL, 'w' },
    { NULL,               0, NULL, 0   }
};

/* Description of short options for getopt_long.  */
static const char* const shortOptions = "b:c:hl:m:n:r:s:t:vw:";

/* Usage summary text.  */
static const char* const usageTemplate =
    "Usage: %s [ options ]\n"
        "  -b, --broadcasters number       Number of broadcasting processes.\n"
        "  -c, --cooldown seconds          Duration of cool-down phase (default = 10).\n"
        "  -h, --help                      Print this information.\n"
        "  -l, --wagonMaxLen               Maximum length of wagons (default = 32768)\n"
        "  -m, --measurement seconds       Duration of measurement phase (default = 600).\n"
        "  -n, --number                    Number of participating processes.\n"
        "  -r, --reqOrder                  Required Order (can be 0 for CAUSAL_ORDER, 1 for TOTAL_ORDER or 2 for UNIFORM_TOTAL_ORDER ; default = 2 for UNIFORM_TOTAL_ORDER).\n"
        "  -s, --size bytes                Bytes contained in each application message o-broadcasted.\n"
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
  if (value < 0)
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

/* Broadcasts a PING message */
void broadcastPing(){
  struct timeval sendTime;
  int rc;
  message *mp = NULL;
  if (size >= sizeof(struct timeval)) {
    mp = newmsg(size);
  } else {
    mp = newmsg(sizeof(struct timeval));
  }
  if (mp == NULL) {
    trError_at_line(-1, trErrno, __FILE__, __LINE__, "newPingMsg()");
    exit(EXIT_FAILURE);
  }

  gettimeofday(&sendTime, NULL);
  *(struct timeval*)(mp->payload) = sendTime;

  if ((rc = oBroadcast(PING, mp)) < 0) {
    trError_at_line(rc, trErrno, __FILE__, __LINE__, "oBroadcast()");
    exit(EXIT_FAILURE);
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

  lastView = *cp;

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
    printf("!!! ******** enough members to start oBroadcasting\n");

    // The experience starts
    int rc = sem_post(&semWaitEnoughMembers);
    if (rc)
      ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "sem_post()");
  }
}

/* Callback for messages to be O-delivered */
void callbackODeliver(address sender, t_typ messageTyp, message *mp){
  char s[MAX_LEN_ADDRESS_AS_STR];
  static address senderOfInitialPing;
  static int nbWaitedPong;
  static int nbRecMsg = 0;

  if (messageTyp == PING) {
    int rc;
    message *pongMsg = newmsg(size);
    if (pongMsg == NULL) {
      trError_at_line(-1, trErrno, __FILE__, __LINE__, "newPongMsg()");
      exit(EXIT_FAILURE);
    }
    senderOfInitialPing = sender;
    nbWaitedPong = number;

    *(struct timeval *)(pongMsg->payload) = *(struct timeval *)(mp->payload);
      
    if ((rc = oBroadcast(PONG, pongMsg)) < 0) {
      trError_at_line(rc, trErrno, __FILE__, __LINE__, "oBroadcast()");
      exit(EXIT_FAILURE);
    }
  } else if (messageTyp == PONG) {
    --nbWaitedPong;
    if (nbWaitedPong == 0) {
      if (addrIsMine(senderOfInitialPing)) {
	struct timeval sendDate, receiveDate, latency;
	sendDate = *(struct timeval*)(mp->payload);
	gettimeofday(&receiveDate, NULL );
	timersub(&receiveDate, &sendDate, &latency);
	if (measurementPhase) {
	  recordValue(latency, &record);
	}
      } else {
	// Compute rank of senderOfInitialPing in lastCircuitView. Thus, we will be able to
	// check if current process is successor of senderOfInitialPing. If it is the
	// case, send a PING message.
	int rankSenderOfInitialPing;
	for (rankSenderOfInitialPing = 0; (rankSenderOfInitialPing < lastView.cv_nmemb) && !addrIsEqual(senderOfInitialPing,lastView.cv_members[rankSenderOfInitialPing]); ++rankSenderOfInitialPing)
	  ;
	if (rank == (rankSenderOfInitialPing + 1) % lastView.cv_nmemb) {
	  broadcastPing();
	}
      }
    }
  } else {
    /* PLAIN message */
    nbRecMsg++;
    if (verbose)
      printf("!!! %5d-th PLAIN message (received from %s / contents = %5d)\n", nbRecMsg,
	     addrToStr(s, sender), *((int*) (mp->payload)));
  }
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
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "gettimeofday");
  if (getrusage(RUSAGE_SELF, &rusageBegin) < 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "getrusage");
  countersBegin = counters;

  measurementPhase = true;
  usleep(measurement * 1000000);

  if (gettimeofday(&timeEnd, NULL ) < 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "gettimeofday");
  if (getrusage(RUSAGE_SELF, &rusageEnd) < 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "getrusage");
  countersEnd = counters;

  measurementPhase = false;
  measurementDone = true;
  // Cool-down phase
  usleep(cooldown * 1000000);

  // We display the results
  printf(
      "%s --broadcasters %d --cooldown %d --wagonMaxLen %d --measurement %d --number %d --size %d --trainsNumber %d  --warmup %d\n",
      programName, broadcasters, cooldown, wagonMaxLen, measurement, number, size,
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

  setStatistics(&record);

  printf(
      "Broadcasters / number / size / ntr / Average number of delivered wagons per recent train received / Average number of msg per wagon / Average size of msg delivered / Throughput of o-broadcasts in Mbps / %%CPU / Number of PING sent by this process / Average of PING-lastPONG time (ms); %d ; %d ; %d ; %d ; %g ; %g ; %g ; %g ; %g ; %u ; %.2lf\n",
      broadcasters, number, size, ntr,
      ((double) (countersEnd.wagons_delivered - countersBegin.wagons_delivered))
          / ((double) (countersEnd.recent_trains_received
              - countersBegin.recent_trains_received)),
      ((double) (countersEnd.messages_delivered
          - countersBegin.messages_delivered))
          / ((double) (countersEnd.wagons_delivered
              - countersBegin.wagons_delivered)),
      ((double)(countersEnd.messages_bytes_delivered - 
		countersBegin.messages_bytes_delivered)) / 
      ((double)(countersEnd.messages_delivered -
		countersBegin.messages_delivered)),
      ((double) (countersEnd.messages_bytes_delivered
          - countersBegin.messages_bytes_delivered) * 8)
          / ((double) (diffTimeval.tv_sec * 1000000 + diffTimeval.tv_usec)),
      ((double) (diffCPU.tv_sec * 1000000 + diffCPU.tv_usec)
       / (double) (diffTimeval.tv_sec * 1000000 + diffTimeval.tv_usec)),
      record.currentRecordsNb,
	 record.mean);

  // Latency results
  printf("\n"
      "Number of PING sent by this process: %u\n"
      "Average of PING-lastPONG time (ms) : %.2lf\n"
      "Variance                           : %lf\n"
      "Standard deviation                 : %lf\n"
      "95%% confidence interval           : [%.2lf ; %.2lf]\n"
      "99%% confidence interval           : [%.2lf ; %.2lf]\n", record.currentRecordsNb,
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
  record = newPingRecord();

  rc = sem_init(&semWaitEnoughMembers, 0, 0);
  if (rc)
    ERROR_AT_LINE(rc, errno, __FILE__, __LINE__, "sem_init()");

  // We initialize the trains protocol
  if (gettimeofday(&timeTrInitBegin, NULL ) < 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "gettimeofday");
  rc = trInit(trainsNumber, wagonMaxLen, 0, 0, callbackCircuitChange, callbackODeliver, reqOrder);
  if (rc < 0) {
    trError_at_line(rc, trErrno, __FILE__, __LINE__, "tr_init()");
    exit(EXIT_FAILURE);
  }
  if (gettimeofday(&timeTrInitEnd, NULL ) < 0)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "gettimeofday");

  // We wait until there are enough members
  do {
    rc = sem_wait(&semWaitEnoughMembers);
  } while ((rc < 0) && (errno == EINTR));
  if (rc)
    ERROR_AT_LINE(EXIT_FAILURE, errno, __FILE__, __LINE__, "sem_wait()");

  // We start the warm-up phase
  rc = pthread_create(&thread, NULL, timeKeeper, NULL );
  if (rc < 0)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  rc = pthread_detach(thread);
  if (rc < 0)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");

  // If process is the first process of the view, it sends the initial
  // PING message.
  if (rank == 0) {
    broadcastPing();
  }

  // We check if process should be a broadcasting process
  if (rank < broadcasters) {
    // It is the case. We broadcast PLAIN messages.
    do {
      message *mp = newmsg(size);
      if (mp == NULL) {
	trError_at_line(rc, trErrno, __FILE__, __LINE__, "newmsg()");
	exit(EXIT_FAILURE);
      }
      rankMessage++;
      *((int*) (mp->payload)) = rankMessage;
      if ((rc = oBroadcast(PLAIN, mp)) < 0) {
        trError_at_line(rc, trErrno, __FILE__, __LINE__, "oBroadcast()");
        exit(EXIT_FAILURE);
      }
    } while (1);
  } else {
    // Sleep enough time to be sure that this thread does not exist
    // before all other jobs are done
    sleep(warmup + measurement + cooldown + 1);
  }
}

int main(int argc, char *argv[]){
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

    case 'r':
      /* User specified -r or --reqOrder.  */
      reqOrder = optArgToCorrectValue();
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

  if (size < minMessageSize){
    fprintf(stderr, "Warning: Size parameter must be at leat %d. As specified size is %d, size set to %d\n",
	    minMessageSize,
	    size,
	    minMessageSize);
    size = minMessageSize;
  }

  if ((reqOrder < CAUSAL_ORDER) || (reqOrder > UNIFORM_TOTAL_ORDER)) {
    fprintf(stderr, "Error : reqOrder parameter is %d (while it can be only 0 for CAUSAL_ORDER, 1 for TOTAL_ORDER or 2 for UNIFORM_TOTAL_ORDER\n", reqOrder);
    return EXIT_FAILURE;
  }    

  /* Initialize data external to this mudule */
  ntr = trainsNumber;
  wagonMaxLen = alternateMaxWagonLen;

  if (wagonMaxLen < size){
    fprintf(stderr, "Warning: wagonMaxLength parameter too small (< size parameter) ==> results will not be significative\n");
  }

  /* We can start the test */
  startTest();

  return EXIT_SUCCESS;
}

