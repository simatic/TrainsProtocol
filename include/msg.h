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
#include "param.h"


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
* @brief Gives the first wagon of @a msg
* @param[in] msg A pointer on message
* @return A pointer on first wagon contained in message of train type
* @note If no wagon returns NULL
*/
wagon* firstWagon(Msg * msg);

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

#endif /* _MSG_H */
