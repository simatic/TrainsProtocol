/**
 * @brief File holding the types' definition for the in/out flux
 * @file msg.h
 * @author Nathan REBOUD & Damien GRAUX
 * @date 30/05/2012
 */
 
#ifndef _MSG_H
#define _MSG_H
 
#include <pthread.h>
#include <assert.h>
#include "address.h"
#include "wagon.h"
#include "common.h"

#define ntr 5


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
 * @brief Data structure for Default messages
 * @note Use only to init message and to coerce the right type in case of bad use of the functions
 */
typedef struct {
  int problem_id; /**<To refer the problem*/
}Default;

/**
 * @brief Data structure for Insert incoms
 */ 
typedef struct {
  address sender; /**<The reference to the sender*/
}Insert;

/**
 * @brief Data structure for AckInsert incoms
 */ 
typedef struct {
  address sender;/**<The reference to the sender*/
}AckInsert;

/**
 * @brief Data structure for NakInsert incoms
 */ 
typedef struct {
  address sender;/**<The reference to the sender*/
}NakInsert;

/**
 * @brief Data structure for NewSucc incoms
 */ 
typedef struct {
  address sender;/**<The reference to the sender*/
}NewSucc;

/**
 * @brief Data structure for Disconnect incoms
 */ 
typedef struct {
  address sender;/**<The reference to the sender>*/
}Disconnect;

/**
 * @brief Data structure for MType
 */ 
typedef enum {
  DEFAULT, /**<USe to init message or to raise error*/
  TRAIN,
  INSERT,
  ACK_INSERT,
  NAK_INSERT,
  DISCONNECT,
  NEWSUCC
}MType;

/**
 * @brief Data structure for Msg
 */ 
typedef struct {
  
  int len;/**<total length of the msg*/
  MType type;
	
  union {
    Default def;
    Train train;
    Insert insert;
    AckInsert ackInsert;
    NakInsert nakInsert;
    NewSucc newSucc;
    Disconnect disconnect;
  }body;  
}Msg;

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
 * @note Used only for Messages and WagonToSend. In fact, when wagons are part of a train they do not bring a @a womim structure, they are part of a bigger structure called Msg.
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
 * @note @a ntr refers to maximal number of trains possible -> Define in constant.h
 */
typedef lts_struct lts_array[ntr];

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
 * @brief Initiate an empty Msg with DEFAULT MType
 * @return An empty Msg
 * @note The problem_id is 0
 */
Msg init_msg();

/**
 * @brief Create a Msg of the given @a mtype
 * @note If @a mtype is TRAIN then it will give a DEFAULT Msg with the problem_id number 1. This function shouldn't be used to create TRAIN.
 * @param[in] mtype The MType wanted
 * @param[in] addr_id The address (short)of the process which is bounded to send the Msg
 * @return The freshly created Msg
 * @warning
 * <ul><li> If an error occured during the creation of the message, the result will be a Msg with a Default body where the problem_id will be -1. In addition, an error will be rised thanks to perror.</li></ul>
 */
Msg newMsg(MType mtype, address addr_id);

/**
 * @brief Create a new wiw
 * @return A pointer on a wiw
 */
wiw * newwiw();

/**
 * @brief Allocate a message
 * @param[in] pw A double pointer on a wiw
 * @param[in] payloadSize An int
 * @return A pointer on a message
 */
message * mallocwiw(wiw **pw, int payloadSize);

/**
 * @brief Look after the counter and free the wiw if it is equal to 0
 * @param[in] ww A pointer on a wiw used to have prefixe and the rest.
 */
void free_wiw(wiw * ww);

#endif /* _MSG_H */
