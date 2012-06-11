/**
* @brief This file contains all the parameters of the protocol
* @file param.h
* @author Nathan REBOUD & Damien GRAUX
* @date 05/06/2012
*/
#ifndef _PARAM_H
#define _PARAM_H

/**
 * @brief Localisation of the file where addresses are written
 */
#define LOCALISATION "./addr_file"/**<File's here.*/

#define NR 3
#define MAX_NTR 16 // 16, because we can code at most 16 addresses in the circuit


#define LOCAL_HOST "localhost"
#define PORT "4242" //FIXME -> Are you sure?
#define CONNECT_TIMEOUT 2000


extern int ntr;
extern int wait_nb_max;
extern int wait_default_time; /**<in microsecond*/



#endif
