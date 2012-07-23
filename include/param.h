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
