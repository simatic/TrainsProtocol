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
 * @brief Definitions related to addresses used in trains protocol.
 * @file address.h
 * @author Michel SIMATIC - Nathan REBOUD - Damien GRAUX
 * @date 26 june 2012
 */

#ifndef _ADDRESS_H
#define _ADDRESS_H

#include "common.h"

/**
 * @brief Maximal number of participants
 */
#define NP 16


/**
 * @brief Address of a process in the trains protocol
 */
typedef unsigned short address;

/**
 * @brief Set of addresses of process
 */
typedef address addressSet;

/**
 * @brief Max length of string which can be filled up by add2str
 */
#define MAX_LEN_ADDRESS_AS_STR 4 /* If the string starts with "#", followed by the rank of the address (maximum 2 digits, as rank of address is between 0 and (MAX_MEMB-1). ), and '\0' after them, we get 4 bytes. */

/**
 * @brief Address of the current process in the train protocol
 */
extern address myAddress;

/**
 * @brief Address for which @a addrIsNull() is true
 */
extern address nullAddress;

/**
 * @brief Macro to test whether address @a ad is null (i.e. it does not correspond to any existing process) or not
 */
#define addrIsNull(ad) ((ad) == 0)

/**
 * @brief Macro to test whether address @a a1 is lower or equal to address @a a2
 */
#define addrCmp(a1,a2) ((a1) <= (a2))

/**
 * @brief Macro to test whether address @a a1 is equal to address @a a2
 */
#define addrIsEqual(a1,a2) ((a1) == (a2))

/**
 * @brief Macro to test whether address @a ad is equal to address @a myAddress
 */
#define addrIsMine(ad) (addrIsEqual((ad),myAddress))

/**
 * @brief Maximum number of members in the protocol
 */
#define MAX_MEMB (8 * sizeof(address)) /* An address is one bit set in address field. So we have at most (8 * sizeof(address)) addresses. */

/**
 * @brief Stores in @a s the (null-terminated) string representation of @a ad
 * @param[in,out] s Array of chars of at least MAX_LEN_ADDRESS_AS_STR bytes 
 * @param[in] ad Address to convert
 * @return @a s
 */
char *addrToStr(char *s, address ad);

/**
 * @brief Returns the rank of address @a ad
 * @param[in] ad Address to convert
 * @return the corresponding rank (value between 0 and (@a MAX_MEMB-1)) or -1 if @a addrIsNull(ad) is true
 */
int addrToRank(address ad);

/**
 * @brief Returns the address corresponding to rank @a rank
 * @param[in] rank Rank to convert
 * @return the corresponding address if @a rank is between 0 and (@a MAX_MEMB-1) and nullAddress otherwise
 */
address rankToAddr(int rank);

/**
 * @brief Tests if @a ad is member of addresses @a adSet
 * @param[in] ad Address to test
 * @param[in] adSet Address set on which to test
 * @return true if @a ad is member of @a adSet. False otherwise
 */
bool addrIsMember(address ad, addressSet adSet);

/**
 * @brief Appends arrived process @a ad to set of addresses @a arrivedSet
 * @param[in,out] arrivedSet Set of processes which arrived
 * @param[in] ad Address of arrived process
 */
void addrAppendArrived(addressSet *arrivedSet, address ad);

/**
 * @brief Appends gone process @a ad to set of addresses @a goneSet (NB: removes @a ad from @a arrivedSet if @a ad is part of @a arrivedSet)
 * @param[in,out] arrivedSet Set of processes which arrived
 * @param[in,out] goneSet Set of processes which gone
 * @param[in] ad Address of gone process
 */
void addrAppendGone(addressSet *arrivedSet, addressSet *goneSet, address ad);

/**
 * @brief Computes new circuit starting from @a circuit, adding addresses in @a arrivedSet before @a ad, and removing addresses in @a goneSet
 * @param[in] circuit Circuit on which to make computation
 * @param[in] ad Address of a process
 * @param[in] arrivedSet Set of processes which arrived
 * @param[in] goneSet Set of processes which gone
 * @return updated circuit 
 */
addressSet addrUpdateCircuit(addressSet circuit, address ad, addressSet arrivedSet, addressSet goneSet);



/**
 * @brief Return the prec address of process @a ad among processes of
 *        circuit @a circuit
 * @param[in] ad the address from which to search the predecessor
 * @param[in] circuit the set of addresses in which to search the predecessor
 * @return the prec address 
 */
address addrPrec(address ad, addressSet circuit);

#endif /* _ADDRESS_H */
