/**
 * @brief Definitions related to the management of addresses used in trains protocol.
 * @file management_addr.h
 * @author Damien GRAUX
 * @date 30/05/2012 
 */

#ifndef _MANAGEMENT_ADDRESS_H
#define _MANAGEMENT_ADDRESS_H

#include "comm.h"

/**
 * @brief Localisation of the file where addresses are written
 */
#define LOCALISATION "./addr_file" /**<File's here.*/


/**
 * @brief Structure of the file addresses variable
 */
typedef struct addr
{
  char* ip;    /**<String holding the ip address*/
  char* chan;  /**<String holding the channel*/
  t_comm* tcomm;/**<t_comm pointer to have the file descriptor*/
}ADDR;

/**
 * @brief Create an empty ADDR
 * @return An empty ADDR
 */
ADDR init_addr(void);

/**
 * @brief Create an array of @a length empty ADDR
 * @param[in] length The length of the array
 * @return The ADDR* created
 */
ADDR* init_addr_list(int length);

/**
 * @brief Clean the ADDR* @a tab
 * @param[in] tab The ADDR* we want to free
 */
void free_addr_list(ADDR* tab);

/**
 * @brief Generate the ADDR* from the file at @a locate
 * @param[in] locate The place where is the file holding the addresses
 * @param[in] length The maximum number of addresses it could have
 * @return The ADDR*
 */
ADDR* addr_generator(char* locate, int length);

/**
 * @brief Add a @a tcomm to @a array at the @a i place
 * @param[in] tcomm The t_comm
 * @param[in] i The place where @a tcomm will be add
 * @param[in] array The ADDR*
 */
void add_tcomm(t_comm * tcomm, int i, ADDR * array);

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
int addr_id(char * ip, char * chan, ADDR * array);

/**
 * @brief Definition of the variable where the addresses will be kept
 * @note Before the utilisation, one should generate it...
 */
ADDR* global_addr_array; 

#endif /* _MANAGEMENT_ADDRESS_H_ */
