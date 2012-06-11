/**
 * @brief Definitions of the complex structures used to manage properly the pointers
 * @file advanced_struct.h
 * @author Damien GRAUX
 * @date 05/06/2012
 */

#ifndef _ADVANCED_STRUCT_H
#define _ADVANCED_STRUCT_H

#include <pthread.h>
#include "msg.h"

/**
 * @brief Descritpion of the structure which will be placed before the messages
 * @note Holds two parameters : a @a mutex to control the writting and a @a counter
 */
typedef struct{
  pthread_mutex_t mutex;/**<To make the writting and the reading of the train safe*/
  int counter;/**<Count the number of pointers refering to the wagons of the train to know when erase the train*/
}prefix;

/**
 * @brief Data structure used to improve the messages and wagon_to_send with a prefixe
 * @note Stand for "Wagon Or Message In Memory"
 * @note Used only for Messages and WagonToSend. In fact, when wagons are part of a train they do not bring a @a womim struc
ture, they are part of a bigger structure called Msg.
 */
typedef struct{
  prefix pfx;
  union{
    wagon wagon;
    Msg msg;
  };
}womim; // womim = Wagon Or Message In Memory

/**
 * @brief A wagon in a womim
 * @note Structure to watch the behaviour or wagon_to_send and messages
 * @note It links wagon_to_send and messages and the prefixe head-on the train
 */
typedef struct {
  wagon* p_wagon;
  womim* p_womim;
}wiw;

/**
 * @brief Data structure for lts
 * @note lts is for Last Train Send
 */
typedef struct {
  stamp stamp;
  address_set circuit;
  struct {
    wiw* w_w;
    int len;
  } w;/**<The area used to stock wagons*/
  wiw* p_wtosend;/**<refers to the wagon which is bouned to be sent*/
}lts_struct;

/**
 * @brief Definition of an array of lts_struct
 * @note @a MAX_NTR refers to maximal number of trains possible -> Define in constant.h
 */
typedef lts_struct lts_array[MAX_NTR];

/**
 * @brief Go to the next wagon
 * @param[in] msg_ext A pointer on an womim whom union represents a Msg
 * @param[in] w The wagon whom we need the next
 * @return A pointer on the next wagon
 * @note The result NULL is rised if the end is reached
 */
wagon* nextWagon(womim* msg_ext, wagon* w);

/**
 * @brief Tests if ad is member of all the circuit of lts.
 * @param[in] ad The address to test
 * @param[in] ltsarray The lts of the protocol
 * @return A boolean
 */
bool is_in_lts(address  ad, lts_array ltsarray);

/**
 * @brief Says if the train signalized by @a tr_st is recent or no
 * @param[in] tr_st The stamp of the train
 * @param[in] plts_array The last train sent
 * @param[in] last_id The last id sent
 * @param[in] nb_train
 * @return A boolean
 * @note This fun holds the overflows
 */
bool is_recent_train(stamp tr_st,lts_array * plts_array, char last_id, int nb_train);
/**
 * @brief Create a new wiw
 * @return A pointer on a wiw
 */
wiw * newwiw();

/**
 * @brief Allocate a message in @a wagonToSend
 * @param[in] payloadSize An int
 * @return A pointer on a message
 */
message * mallocwiw(int payloadSize);

/**
 * @brief Look after the counter and free the wiw if it is equal to 0
 * @param[in] ww A pointer on a wiw used to have prefixe and the rest.
 */
void free_wiw(wiw * ww);

/**
 * @brief Wagon containing the next messages to be uto-broadcasted
 */
extern wiw * wagonToSend;

/**
 * @brief Mutex protecting access to wagonToSend
 */
extern pthread_mutex_t mutexWagonToSend;

/**
 * @brief Condition protecting access to wagonToSend
 */
extern pthread_cond_t condWagonToSend;


#endif /* _ADVANCED_STRUCT_H */
