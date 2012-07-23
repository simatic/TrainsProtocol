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
 * @date  15 april 2012
 */

#ifndef _SIGNALMGT_H_
#define _SIGNALMGT_H_

#include <signal.h>

// Signal used to manage I/O
#define SIGNAL_FOR_ABORT (SIGRTMIN + 1)

/**
 * @brief Initializes signalMgt module
 */
void signalMgtInitialize();

#endif /* _SIGNALMGT_H_ */

