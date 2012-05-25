/**
 * @brief Definitions related to addresses used in trains protocol.
 */

#ifndef _ADDRESS_H
#define _ADDRESS_H

#include "common.h"

/**
 * @brief Address of a process in the trains protocol
 */
typedef unsigned short address;

/**
 * @brief Set of addresses of process
 */
typedef address address_set;

/**
 * @brief Max length of string which can be filled up by add2str
 */
#define MAX_LEN_ADDRESS_AS_STR 4 /* If the string starts with "#", followed by the rank of the address (maximum 2 digits, as rank of address is between 0 and (MAX_MEMB-1). ), and '\0' after them, we get 4 bytes. */

/**
 * @brief Address of the current process in the train protocol
 */
extern address my_address;

/**
 * @brief Address for which @a addr_isnull() is true
 */
extern address null_address;

/**
 * @brief Macro to test whether address @a ad is null (i.e. it does not correspond to any existing process) or not
 */
#define addr_isnull(ad) ((ad) == 0)

/**
 * @brief Macro to test whether address @a a1 is lower or equal to address @a a2
 */
#define addr_cmp(a1,a2) ((a1) <= (a2))

/**
 * @brief Macro to test whether address @a a1 is equal to address @a a2
 */
#define addr_isequal(a1,a2) ((a1) == (a2))

/**
 * @brief Macro to test whether address @a ad is equal to address @a my_address
 */
#define addr_ismine(ad) (addr_isequal((ad),my_address))

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
char *addr_2_str(char *s, address ad);

/**
 * @brief Returns the rank of address @a ad
 * @param[in] ad Address to convert
 * @return the corresponding rank (value between 0 and (@a MAX_MEMB-1)) or -1 if @a addr_isnull(ad) is true
 */
int addr_2_rank(address ad);

/**
 * @brief Returns the address corresponding to rank @a rank
 * @param[in] rank Rank to convert
 * @return the corresponding address if @a rank is between 0 and (@a MAX_MEMB-1) and null_address otherwise
 */
address rank_2_addr(int rank);

/**
 * @brief Tests if @a ad is member of addresses @a adSet
 * @param[in] ad Address to test
 * @param[in] adSet Address set on which to test
 * @return true if @a ad is member of @a adSet. False otherwise
 */
bool addr_ismember(address ad, address_set adSet);

/**
 * @brief Appends arrived process @a ad to set of addresses @a arrivedSet
 * @param[in,out] arrivedSet Set of processes which arrived
 * @param[in] ad Address of arrived process
 */
void addr_appendArrived(address_set *arrivedSet, address ad);

/**
 * @brief Appends gone process @a ad to set of addresses @a goneSet (NB: removes @a ad from @a arrivedSet if @a ad is part of @a arrivedSet)
 * @param[in,out] arrivedSet Set of processes which arrived
 * @param[in,out] goneSet Set of processes which gone
 * @param[in] ad Address of gone process
 */
void addr_appendGone(address_set *arrivedSet, address_set *goneSet, address ad);

/**
 * @brief Computes new circuit starting from @a circuit, adding addresses in @a arrivedSet before @a ad, and removing addresses in @a goneSet
 * @param[in] circuit Circuit on which to make computation
 * @param[in] ad Address of a process
 * @param[in] arrivedSet Set of processes which arrived
 * @param[in] goneSet Set of processes which gone
 * @return updated circuit 
 */
address_set addr_updateCircuit(address_set circuit, address ad, address_set arrivedSet, address_set goneSet);

#endif /* _ADDRESS_H */
