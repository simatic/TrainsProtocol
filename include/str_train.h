/**
 * @brief Definitions of all the types related to trains
 * @file str_train.h
 * @author Damien GRAUX & Nathan REBOUD
 * @date 30/05/2012
 */

#ifndef _STR_TRAIN_H
#define _STR_TRAIN_H

#include <pthread.h>
#include "address.h"
#include "wagon.h"
#include "common.h"

#define ntr 5 //FIXME : waiting for a migration to constant.h



/**
 * @brief Data structure for stamps
 */
typedef struct {
  char id;
  char lc;
  char round;
}stamp;

/**
 * @brief Data structure for trains
 */
typedef struct {
  stamp stamp;/**<A stramp for some info*/
  address_set circuit;/**<A description of the circuit*/
  wagon wagons[];/**<The block of wagons*/
} Train;

/**
 * @brief Descritpion of the structure which will be placed before the trains
 * @note Holds two parameters : a mutec to control the writting and a counter
 */
typedef struct{
  pthread_mutex_t mutex;/**<To make the writting and the reading of the train safe*/
  int counter;/**<Count the number of pointers refering to the wagons of the train to know when erase the train*/
  int len;/**<Length of the train which is behind the prefix*/
} prefix;

/**
 * @brief Data structure used to improve the trains with a prefixe
 */
typedef struct {
  prefix pfx;
  Train train;
}train_extended;

/**
 * @brief Structure to watch the behaviour or wagon
 * @note it links wagon and the prefixe head-on the train
 */
typedef struct {
  prefix* p_pfx;
  wagon* p_wagon;
} wagon_watcher;

/**
 * @brief Data structure for lts
 * @note lts is for Last Train Send
 */
typedef struct {
  stamp stamp;
  address_set circuit;
  struct {
    wagon_watcher* w_w;
    int len;
  } w;/**<The area used to stock wagons*/
  wagon_watcher* p_wtosend;/**<refers to the wagon which is bouned to be sent*/
}lts_struct;

/**
 * @brief Definition of an array of lts_struct
 * @note @a ntr refers to maximal number of trains possible -> Define in constant.h
 */
typedef lts_struct lts_array[ntr];

/**
 * @brief Go to the next wagon
 * @param[in] t A pointer on an extended train
 * @param[in] w The wagon whom we need the next
 * @return A pointer on the next wagon
 * @note The result NULL is rised if the end is reached
 */
wagon* nextWagon(train_extended* t, wagon* w);

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
 * @param[in] lis The last instance sent
 * @param[in] nb_train
 * @return A boolean
 * @note This fun holds the overflows
 */
bool is_recent_train(stamp tr_st, lts_array * plts_array, char lis, int nb_train);

/**
 * @brief Look after the counter and free the Train if it is equal to 0
 * @param[in] ww A wagon_watcher used to have prefixe and wagons
 */
void free_wagon(wagon_watcher ww);

#endif /* _STR_TRAIN_H */

