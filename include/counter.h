/**
 * @brief Definitions related to counters.
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
  t_counter bytes_received;               /**< number of bytes received from the network */
  t_counter messages_delivered;           /**< number of messages delivered to the application */
  t_counter messages_bytes_delivered;     /**< number of bytes delivered to the application */
  t_counter recent_trains_bytes_received; /**< number of bytes of recent trains received from the network */
  t_counter recent_trains_received;       /**< number of recent trains received from the network */
  t_counter trains_bytes_received;        /**< number of bytes of trains received from the network */
  t_counter trains_received;              /**< number of trains received from the network */
  t_counter wagons_delivered;             /**< number of wagons delivered to the application */
  t_counter wait_states;                  /**< number of times automaton has been in state WAIT */
  t_counter comm_read;                    /**< number of calls to comm_read() */
  t_counter comm_read_bytes;              /**< number of bytes read by comm_read() calls */
  t_counter comm_readFully;               /**< number of calls to comm_readFully() */
  t_counter comm_readFully_bytes;         /**< number of bytes read by comm_readFully() calls */
  t_counter comm_write;                   /**< number of calls to comm_write() */
  t_counter comm_write_bytes;             /**< number of bytes written by comm_write() calls */
  t_counter comm_writev;                  /**< number of calls to comm_writev() */
  t_counter comm_writev_bytes;            /**< number of bytes written by comm_writev() calls */
  t_counter newmsg;                       /**< number of calls to newmsg() */
  t_counter flowControl;                  /**< number of times there was flow control when calling newmsg() */
} t_counters;

/** 
 * @brief Variable containing all counters
 */
extern t_counters counters;


#endif /* _counters_H */
