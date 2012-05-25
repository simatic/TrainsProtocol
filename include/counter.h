/**
 * @brief Definitions related to counters.
 */

#ifndef _counters_H
#define _counters_H

/** 
 * @brief Type used to store counters
 */
typedef unsigned long long t_counter;

/** 
 * @brief Counter for bytes received from the network
 */
extern t_counter counter_bytes_received;

/** 
 * @brief Counter for bytes of trains received from the network
 */
extern t_counter counter_trains_bytes_received;

/** 
 * @brief Counter for number of trains received from the network
 */
extern t_counter counter_trains_received;

/** 
 * @brief Counter for bytes of recent trains received from the network
 */
extern t_counter counter_recent_trains_bytes_received;

/** 
 * @brief Counter for number of recent trains received from the network
 */
extern t_counter counter_recent_trains_received;

/** 
 * @brief Counter for number of wagons delivered to the application
 */
extern t_counter counter_wagons_delivered;

/** 
 * @brief Counter for number of messages delivered to the application
 */
extern t_counter counter_messages_delivered;

/** 
 * @brief Counter for number of bytes delivered to the application
 */
extern t_counter counter_messages_bytes_delivered;

/** 
 * @brief Counter for number of times automaton is in state WAIT
 */
extern t_counter counter_wait_states;

#endif /* _counters_H */
