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
 * @brief Definitions related to counters.
 * @file counter.h
 * @author Michel SIMATIC
 * @date 26 june 2012
 */

#ifndef _counters_H
#define _counters_H

/** 
 * @brief Type used to store a single counter
 */
typedef unsigned long long t_counter;

/** 
 * @brief Type used to store all counters
 */
typedef struct{
  t_counter messages_delivered;           /**< number of messages delivered to the application */
  t_counter messages_bytes_delivered;     /**< number of bytes delivered to the application */
  t_counter trains_bytes_received;        /**< number of bytes of trains received from the network */
  t_counter trains_received;              /**< number of trains received from the network */
  t_counter recent_trains_bytes_received; /**< number of bytes of recent trains received from the network */
  t_counter recent_trains_received;       /**< number of recent trains received from the network */
  t_counter wagons_delivered;             /**< number of wagons delivered to the application */
  t_counter wait_states;                  /**< number of times automaton has been in state WAIT */
  t_counter comm_read;                    /**< number of calls to commRead() */
  t_counter comm_read_bytes;              /**< number of bytes read by commRead() calls */
  t_counter comm_readFully;               /**< number of calls to commReadFully() */
  t_counter comm_readFully_bytes;         /**< number of bytes read by commReadFully() calls */
  t_counter comm_write;                   /**< number of calls to commWrite() */
  t_counter comm_write_bytes;             /**< number of bytes written by commWrite() calls */
  t_counter comm_writev;                  /**< number of calls to commWritev() */
  t_counter comm_writev_bytes;            /**< number of bytes written by commWritev() calls */
  t_counter newmsg;                       /**< number of calls to newmsg() */
  t_counter flowControl;                  /**< number of times there was flow control when calling newmsg() */
} t_counters;

/** 
 * @brief Variable containing all counters
 */
extern t_counters counters;


#endif /* _counters_H */
