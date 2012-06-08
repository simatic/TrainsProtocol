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
#include <error.h>
#include <errno.h>
#include <semaphore.h>
#include <strings.h>
#include <pthread.h>
#include "trains.h"
#include "counter.h"

/* The following declaration should be ioncluded in train.h */
extern int ntr;

/* Semaphore used to block main thread until there are enough participants */
static sem_t semWaitEnoughMembers;

/* Rank of the process in the group of particpants when there are enough
   participants. */
static int rank;

/* Boolean indicating that the measurement phase is over */
static bool measurementDone = false;

/* Storage to measure execution time of tr_init */
struct timeval timeTrInitBegin, timeTrInitEnd;

/* Storage of the program name, which we'll use in error messages.  */
char *program_name;

/* Parameters of the program */
int broadcasters = -1;
int cooldown     = 10;    /* Default value = 10 seconds */
int measurement  = 600;   /* Default value = 600 seconds */
int number       = -1;
int size         = -1;
int trainsNumber = -1;
bool verbose     = false; /* Default value = limited display */
int warmup       = 300;   /* Default value = 300 seconds */

/* Description of long options for getopt_long.  */
static const struct option long_options[] = {
  { "broadcasters",     1, NULL, 'b' },
  { "cooldown",         1, NULL, 'c' },
  { "help",             0, NULL, 'h' },
  { "measurement",      1, NULL, 'm' },
  { "number",           1, NULL, 'n' },
  { "size",             1, NULL, 's' },
  { "trainsNumber",     1, NULL, 't' },
  { "verbose",          0, NULL, 'v' },
  { "warmup",           1, NULL, 'w' },
  { NULL,               0, NULL, 0  }
};

/* Description of short options for getopt_long.  */
static const char* const short_options = "b:c:hm:n:s:t:vw:";

/* Usage summary text.  */
static const char* const usage_template = 
  "Usage: %s [ options ]\n"
  "  -b, --broadcasters number       Number of broadcasting processes.\n"
  "  -c, --cooldown seconds          Duration of cool-down phase (default = 10).\n"
  "  -h, --help                      Print this information.\n"
  "  -m, --measurement               Duration of measurement phase (default = 600).\n"
  "  -n, --number                    Number of participating processes.\n"
  "  -s, --size bytes                Bytes contained in each application message uto-broadcasted.\n"
  "  -t, --trainsNumber              Number of trains which should be used by the protocol.\n"
  "  -v, --verbose                   Print verbose messages.\n"
  "  -w, --warmup                    Duration of warm-up phase (default = 300).\n"
;

/* Print usage information and exit.  If IS_ERROR is non-zero, write to
   stderr and use an error exit code.  Otherwise, write to stdout and
   use a non-error termination code.  Does not return. 
*/
static void print_usage (int is_error)
{
  fprintf (is_error ? stderr : stdout, usage_template, program_name);
  exit (is_error ? EXIT_FAILURE : EXIT_SUCCESS);
}

/* Converts the value stored in optarg into an integer.
   Calls print_usage is conversion is not possible or if the
   value is incorrect.
*/
int optarg2correctValue(){
  long value;
  char* end;

  value = strtol (optarg, &end, 10);
  if (*end != '\0')
    /* The user specified non-digits for this number.  */
    print_usage (EXIT_FAILURE);
  if (value <= 0 )
    /* The user gave an incorrect value. */
    print_usage (EXIT_FAILURE);
  return value;
}

/* Checks that a parameter (which name is pointed by name) has been given a value (its value is no more negative) */
void check(int value, char *name){
  if (value < 0) {
    fprintf(stderr, "\"%s\" must be specified\n", name);
    print_usage(EXIT_FAILURE);
  }
}

/* Prints message msg, followed by a ";" and the difference between stop and start */
void printDiffTimeval(char *msg, struct timeval stop, struct timeval start){
  struct timeval diffTimeval;
  timersub(&stop, &start, &diffTimeval);
  printf("%s ; %8d.%6d s\n", msg, (int)diffTimeval.tv_sec, (int)diffTimeval.tv_usec);
}

/* Callback for circuit changes */
void callbackCircuitChange(circuitview *cp){
  char s[MAX_LEN_ADDRESS_AS_STR];

  printf("!!! ******** callbackCircuitChange called with %d members (process ", cp->cv_nmemb);
  if (!addr_isnull(cp->cv_joined)){
    printf("%s has arrived)\n", addr_2_str(s,cp->cv_joined));
  }else{
    printf("%s is gone)\n", addr_2_str(s,cp->cv_departed));
    if (!measurementDone) {
      printf("!!! ******** Experience has failed ******** !!!\n");
      exit(EXIT_FAILURE);
    }
  }

  if(cp->cv_nmemb >= number){
    // We compute the rank of the process in the group
    for (rank=0; (rank<cp->cv_nmemb) && addr_ismine(cp->cv_members[rank]); rank++);
    // We can start the experience
    printf("!!! ******** enough members to start utoBroadcasting\n");
    int rc = sem_post(&semWaitEnoughMembers);
    if (rc)
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "sem_post()");
  }
}

/* Callback for messages to be UTO-delivered */
void callbackUtoDeliver(address sender, message *mp){
  char s[MAX_LEN_ADDRESS_AS_STR];
  static int nbRecMsg = 0;

  if (payload_size(mp) != size){
    fprintf(stderr, "Error in file %s:%d : Payload size is incorrect: it is %d when it should be %d\n", 
	    __FILE__,
	    __LINE__,
	    payload_size(mp),
	    size);
    exit(EXIT_FAILURE);
  }

  nbRecMsg++;

  if (verbose)
    printf("!!! %5d-ieme message (recu de %s / contenu = %5d)\n", nbRecMsg, addr_2_str(s,sender), *((int*)(mp->payload)));

}

/* Thread taking care of respecting the times of the experiment. */
void *timeKeeper(void *null) {
  int rc;
  t_counters countersBegin, countersEnd;
  struct rusage rusageBegin, rusageEnd;
  struct timeval timeBegin, timeEnd;
  struct timeval startSomme, stopSomme, diffTimeval;

  // Warm-up phase
  usleep(warmup * 1000000);

  // Measurement phase
  if (gettimeofday(&timeBegin, NULL) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "gettimeofday");
  if (getrusage(RUSAGE_SELF, &rusageBegin) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "getrusage");
  countersBegin = counters;

  usleep(measurement * 1000000);

  if (gettimeofday(&timeEnd, NULL) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "gettimeofday");
  if (getrusage( RUSAGE_SELF, &rusageEnd) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "getrusage");
  countersEnd = counters;

  measurementDone = true;

  // Cool-down phase
  usleep(cooldown * 1000000);

  // We display the results
  printf("%s --broadcasters %d --cooldown %d --measurement %d --number %d --size %d --trainsNumber %d  --warmup %d\n\n", 
	 program_name, broadcasters, cooldown, measurement, number, size, trainsNumber,  warmup);

  printDiffTimeval("time for tr_init (in sec)", timeTrInitEnd, timeTrInitBegin);

  printDiffTimeval("elapsed time (in sec)", timeEnd, timeBegin);

  printDiffTimeval("ru_utime (in sec)", rusageEnd.ru_utime, rusageBegin.ru_utime);
  printDiffTimeval("ru_stime (in sec)", rusageEnd.ru_stime, rusageBegin.ru_stime);

  timeradd(&rusageBegin.ru_utime, &rusageBegin.ru_stime, &startSomme);
  timeradd(&rusageEnd.ru_utime, &rusageEnd.ru_stime, &stopSomme);
  printDiffTimeval("ru_utime+ru_stime (in sec)", stopSomme, startSomme);

  printf("number of bytes received from the network;%llud\n", countersEnd.bytes_received - countersBegin.bytes_received);
  printf("number of messages delivered to the application;%llud\n", countersEnd.messages_delivered - countersBegin.messages_delivered);
  printf("number of bytes delivered to the application;%llud\n", countersEnd.messages_bytes_delivered - countersBegin.messages_bytes_delivered);
  printf("number of bytes of recent trains received from the network;%llud\n", countersEnd.recent_trains_bytes_received - countersBegin.recent_trains_bytes_received);
  printf("number of recent trains received from the network;%llud\n", countersEnd.recent_trains_received - countersBegin.recent_trains_received);
  printf("number of bytes of trains received from the network;%llud\n", countersEnd.trains_bytes_received - countersBegin.trains_bytes_received);
  printf("number of trains received from the network;%llud\n", countersEnd.trains_received - countersBegin.trains_received);
  printf("number of wagons delivered to the application;%llud\n", countersEnd.wagons_delivered - countersBegin.wagons_delivered);
  printf("number of times automaton has been in state WAIT;%llud\n", countersEnd.wait_states - countersBegin.wait_states);
  printf("number of times comm_read() calls;%llud\n", countersEnd.comm_read - countersBegin.comm_read);
  printf("number of bytes read by comm_read() calls;%llud\n", countersEnd.comm_read_bytes - countersBegin.comm_read_bytes);
  printf("number of times comm_readFully() calls;%llud\n", countersEnd.comm_readFully - countersBegin.comm_readFully);
  printf("number of bytes read by comm_readFully() calls;%llud\n", countersEnd.comm_readFully_bytes - countersBegin.comm_readFully_bytes);
  printf("number of times comm_write() calls;%llud\n", countersEnd.comm_write - countersBegin.comm_write);
  printf("number of bytes written by comm_write() calls;%llud\n", countersEnd.comm_write_bytes - countersBegin.comm_write_bytes);
  printf("number of times comm_writev() calls;%llud\n", countersEnd.comm_writev - countersBegin.comm_writev);
  printf("number of bytes written by comm_writev() calls;%llud\n", countersEnd.comm_writev_bytes - countersBegin.comm_writev_bytes);
  printf("number of times there was flow control when calling newmsg();%llud\n\n", countersEnd.flowControl - countersBegin.flowControl);

  timersub(&timeEnd, &timeBegin, &diffTimeval);
  printf("Broadcasters / number / size / ntr / Average msg per wagon / Throughput of uto-broadcasts in Mbps ; %d ; %d ; %d ; %d ; %g ; %g\n", 
	 broadcasters,
	 number, 
	 size,
	 ntr,
	 ((double)(countersEnd.messages_delivered - countersBegin.messages_delivered)) / ((double)(countersEnd.wagons_delivered - countersBegin.wagons_delivered)),
	 ((double)(countersEnd.messages_bytes_delivered - countersBegin.messages_bytes_delivered) * 8) /
	 ((double)(diffTimeval.tv_sec * 1000000 + diffTimeval.tv_usec)));

  // Termination phase
  rc = tr_terminate();
  if (rc < 0) {
    tr_error_at_line(rc, tr_errno, __FILE__, __LINE__, "tr_init()");
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);

  return NULL;
}

void startTest() {
  int rc;
  int  rankMessage = 0;
  pthread_t thread;

  rc = sem_init(&semWaitEnoughMembers, 0, 0); 
  if (rc)
    error_at_line(rc, errno, __FILE__, __LINE__, "sem_init()");

  // We initialize the trains protocol
  if (gettimeofday(&timeTrInitBegin, NULL) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "gettimeofday");
  rc = tr_init(callbackCircuitChange, callbackUtoDeliver);
  if (rc < 0) {
    tr_error_at_line(rc, tr_errno, __FILE__, __LINE__, "tr_init()");
    exit(EXIT_FAILURE);
  }
  if (gettimeofday(&timeTrInitEnd, NULL) < 0)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "gettimeofday");

  // We wait until there are enough members
  do {
    rc = sem_wait(&semWaitEnoughMembers);
  } while ((rc < 0) && (errno == EINTR));
  if (rc)
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "sem_wait()");

  // We start the warm-up phase
  rc = pthread_create(&thread, NULL, timeKeeper, NULL);
  if (rc < 0)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  rc = pthread_detach(thread);
  if (rc < 0)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
  
  // We check if process should be a broadcasting process
  if (rank < broadcasters){
    // It is the case
    do {
      message *mp = newmsg(size);
      if (mp == NULL){
	tr_error_at_line(rc, tr_errno, __FILE__, __LINE__, "newmsg()");
	exit(EXIT_FAILURE);
      }    
      rankMessage++;
      *((int*)(mp->payload)) = rankMessage;
      if (uto_broadcast(mp) < 0){
	tr_error_at_line(rc, tr_errno, __FILE__, __LINE__, "utoBroadcast()");
	exit(EXIT_FAILURE);
      }
    } while (1);
  }
}

int main(int argc, char *argv[]) {
  int next_option;

  /* Store the program name, which we'll use in error messages.  */
  program_name = argv[0];

  /* Parse options.  */
  do {
    next_option = 
      getopt_long (argc, argv, short_options, long_options, NULL);
    switch (next_option) {
    case 'b':  
      /* User specified -b or --broadcasters.  */
      broadcasters = optarg2correctValue();
      break;

    case 'c':  
      /* User specified -c or --cooldown.  */
      cooldown = optarg2correctValue();
      break;

    case 'h':  
      /* User specified -h or --help.  */
      print_usage (EXIT_SUCCESS);

    case 'm':  
      /* User specified -m or --measurement.  */
      measurement = optarg2correctValue();
      break;

    case 'n':  
      /* User specified -n or --number.  */
      number = optarg2correctValue();
      break;

    case 's':  
      /* User specified -s or --size.  */
      size = optarg2correctValue();
      break;

    case 't':  
      /* User specified -t or --trainsNumber.  */
      trainsNumber = optarg2correctValue();
      break;

    case 'v':  
      /* User specified -v or --verbose.  */
      verbose = true;
      break;

    case 'w':  
      /* User specified -w or --warmup.  */
      warmup = optarg2correctValue();
      break;

    case '?':  
      /* User specified an unrecognized option.  */
      print_usage (EXIT_FAILURE);

    case -1:  
      /* Done with options.  */
      break;

    default:
      abort ();
    }
  } while (next_option != -1);

  /* This program takes no additional arguments.  Issue an error if the
     user specified any.  */
  if (optind != argc)
    print_usage (1);

  /* Check that parameters without default values were specified. */
  check(broadcasters, "broadcasters");
  check(number, "number");
  check(size, "size");
  check(trainsNumber, "trainsNumber");

  /* Initialize data external to this mudule */
  ntr = trainsNumber;

  /* We can start the test */
  startTest();

  return EXIT_SUCCESS;
}






