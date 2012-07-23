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
 * @brief Manage in the messages the process which participate to the protocol on an applicative level
 * @file signalArrival.h
 * @author Damien GRAUX
 * @date 05 june 2012
 */

#ifndef _SIGNAL_ARRIVAL_H
#define _SIGNAL_ARRIVAL_H

#include "advanced_struct.h"

/**
 * @brief Adds an application message to wagon @a w about arrival of process @a arrived
 * @param[in] arrived Process which arrived
 * @param[in] circuit Circuit in which process has arrived (@a arrived process can be mentionned in @a circuit, but it is not mandatory)
 */
void signalArrival(address arrived, addressSet circuit);

/**
 * @brief Adds one application message to wagon @a w per process which adress appears in @a departedSet
 * @param[in] goneSet Set of gone process coded as one bit set per pne process
 * @param[in] circuit Circuit from which processes have gone (Processes in @a departedSet may not appear in @a circuit, but it is not mandatory)
*/
void signalDepartures(addressSet goneSet, addressSet circuit);


#endif /* _SIGNAL_ARRIVAL_H */
