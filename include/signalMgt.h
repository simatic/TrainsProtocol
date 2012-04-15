/**
 * @brief Module designed to handle correctly signals as we are using pthreads.
 *
 * For more information, see:
 * <ul>
 * <li>http://www.cognitus.net/html/howto/pthreadSemiFAQ_8.html</li>
 * <li>http://www.linuxquestions.org/questions/programming-9/sigio-sigaction-wakes-sleep-up-early-738720/ recommends to read David R. Butenhof's book Programming with POSIX Threads, published by Addison Wesley. It says "Always use sigwait to work with asynchronous signals within threaded programs."</li>
 * <li>Christophe Blaess's "Programmation système en C sous Linux" (in French), published by Eyrolles also recommends sigwait (see end of chapter 12).</li>
 * </ul>
 *
 * @file signalMgt.h
 * @author Michel SIMATIC
 * @date  15/04/2012
 */

#ifndef _SIGNALMGT_H_
#define _SIGNALMGT_H_

#include <signal.h>

// Signal used to manage I/O
#define SIGNAL_FOR_ABORT (SIGRTMIN + 1)

/**
 * @brief Initializes signalMgt module
 */
void signalMgt_initialize();

#endif /* _SIGNALMGT_H_ */

