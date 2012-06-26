/**
 * @brief This file contains all the parameters of the protocol
 * @file param.h
 * @author Nathan REBOUD - Damien GRAUX
 * @date 05 june 2012
 */


#ifndef _PARAM_H
#define _PARAM_H

/**
 * @brief Localisation of the file where addresses are written
 */
#define LOCALISATION "./addr_file"/**<File's here.*/

/**
 * @brief The number of rounds used for the modulos
 */
#define NR 3

/**
 * @brief The limit number of processes in the circuit
 * @note Set at 16
 */
#define MAX_NTR 16

/**
 * @brief Some definitions to make the protocol possible
 * @warning The @a LOCAL_HOST declaration is a bit strong we need to have much more flexibility
 */
#define LOCAL_HOST "localhost" // FIXME !!!!
#define PORT "2000" 

/**
 * @brief Time to wait
 */
#define CONNECT_TIMEOUT 2000

/**
 * @brief Number of train allowed in the circuit
 * @note The number should be adjust with care to make the protocol as performant as possible
 */
extern int ntr;

/**
 * @brief The limit of time to wait
 */
extern int wait_nb_max;

/**
 * @brief The default time to wait
 */
extern int wait_default_time; /**<in microsecond*/


#endif
