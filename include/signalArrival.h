/**
 * @brief Manage in the messages the process which participate to the protocol on an applicative level
 * @file signalArrival.h
 * @author Damien GRAUX
 * @date 05/06/2012
 */

#ifndef _SIGNAL_ARRIVAL_H
#define _SIGNAL_ARRIVAL_H

#include "advanced_struct.h"

/**
 * @brief Adds an application message to wagon @a w about arrival of process @a arrived
 * @param[in] arrived Process which arrived
 * @param[in] circuit Circuit in which process has arrived (@a arrived process can be mentionned in @a circuit, but it is not mandatory)
 */
void signalArrival(address arrived, address_set circuit);

/**
 * @brief Adds one application message to wagon @a w per process which adress appears in @a departedSet
 * @param[in] goneSet Set of gone process coded as one bit set per pne process
 * @param[in] circuit Circuit from which processes have gone (Processes in @a departedSet may not appear in @a circuit, but it is not mandatory)
*/
void signalDepartures(address_set goneSet, address_set circuit);


#endif /* _SIGNAL_ARRIVAL_H */
