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
 * @brief Definitions related to the management of addresses used in trains protocol.
 * @file management_addr.h
 * @author Damien GRAUX
 * @date 30 may 2012 
 */

#ifndef _MANAGEMENT_ADDRESS_H
#define _MANAGEMENT_ADDRESS_H

#include "comm.h"
#include "trains.h"
#include "address.h"

/**
 * @brief Max length of a rank (including '\0')
 */
#define MAX_LEN_RANK 2

/**
 * @brief Max length of an IP address (including '\0')
 */
#define MAX_LEN_IP 64

/**
 * @brief Max length of a channel (including '\0')
 */
#define MAX_LEN_CHAN 64

/**
 * @brief Structure of the file addresses variable
 */
typedef struct
{
  char ip[MAX_LEN_IP];    /**<String holding the ip address*/
  char chan[MAX_LEN_CHAN];  /**<String holding the channel*/
  trComm* tcomm[2];/**<trComm pointers to have the file descriptor (we need to store 2 trComm pointers as a process may have 2 connections with another process)*/
  bool isPred[2];/**<indicates if the corresponding tcomm is the one of a predecessor or not */
}ADDR;


//give the length of a char *, or 0 if the char * == NULL
//this function has been created because strlen was not sufficient for our purpose

/**
 * @brief Alternative version of strlen we created because strlen can not evaluate NULL
 * @param[in] stringToEvaluate
 * @return The length of a char*, or 0 if stringToEvaluate == NULL
 */
int stringLength(char * stringToEvaluate);

/**
 * @brief Create an array of @a length empty ADDR
 * @param[in] length The length of the array
 * @return The ADDR* created
 */
ADDR* initAddrList(int length);

/**
 * @brief Clean the ADDR* @a tab
 * @param[in] tab The ADDR* we want to free
 */
void freeAddrList(ADDR* tab);

/**
 * @brief Generate the ADDR* from the file at @a locate
 * @param[in] locate The place where is the file holding the addresses
 * @param[in] length The maximum number of addresses it could have
 * @return The ADDR*
 */
ADDR* addrGenerator(char* locate, int length);

/**
 * @brief Add a @a tcomm to @a array at the @a i place
 * @param[in] tcomm The trComm
 * @param[in] i The place where @a tcomm will be add
 * @param[in] array The ADDR*
 * @param[in] isPred the value the isPred field should have
 */
void addTComm(trComm * tcomm, int i, ADDR * array, bool isPred);

/**
 * @brief Tries to return a non-NULL @a tcomm from @a array at the @a i place
 *        and having the same @a isPred
 * @param[in] i The place where @a tcomm will be add
 * @param[in] isPred the value the isPred field should have
 * @param[in] array The ADDR*
 * @return tcomm[0] if it is non-NULL and has the right isPred, or tcomm[1] 
 *         for the same reasons or NULL
 */
trComm *getTComm(int i, bool isPred, ADDR * array);

/**
 * @brief Remove a @a tcomm from @a array at the @a i place
 * @param[in] tcomm The trComm
 * @param[in] i The place from where @a tcomm will be removed
 * @param[in] array The ADDR*
 */
void removeTComm(trComm * tcomm, int i, ADDR * array);

/**
 * @brief Search if a @a tcomm is present in @a array
 * @param[in] tcomm The trComm to search for
 * @param[in] array The ADDR array to crowl
 * @param[out] prank Rank of the found @a tcomm (-1 if unfound)
 * @param[out] pisPred Set to true if the @a tcomm found is the one of a pred
 */
void searchTComm(trComm * tcomm, ADDR * array, int *prank, bool *pisPred);

/**
 * @brief Give the place in the @a array of a given address
 * @param[in] ip The string of the ip address
 * @param[in] chan The string of the channel
 * @param[in] array The ADDR*
 * @return The index of the address
 * @warning
 * <ul>
 * <li>If the address caracterized by @a ip and @a chan is not found in @a array, this function will return the integer -1.</li>
 * </ul>
 */
int addrID(char * ip, char * chan, ADDR * array);

/**
 * @brief Definition of the variable where the addresses will be kept
 * @note Before the utilisation, one should generate it...
 */
extern ADDR* globalAddrArray; 

#endif /* _MANAGEMENT_ADDRESS_H_ */
