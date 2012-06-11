/**
* @brief This file contains the state machine
* @file stateMachine.h
* @author Nathan REBOUD
* @date 05/06/2012
*/
#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H

#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <unistd.h> //pour la fonction sleep
#include "param.h"
#include "address.h"
#include "iomsg.h"
#include "wagon.h"
#include "common.h"
#include "msg.h"
#include "connect.h"
#include "signalArrival.h"


/**
* @brief Data structure for State
*/
typedef enum {
  OFFLINE_CONNECTION_ATTEMPT, /**<refers to the out of cirucit state when the process aims to connect*/
  OFFLINE_CONFIRMATION_WAIT, /**<the process is waiting for a train which proves that it is in the circuit*/
  ALONE_INSERT_WAIT, /**<when the process is alone in the circuit*/
  ALONE_CONNECTION_WAIT, /**<the proc is alone and recieve an Insert*/
  SEVERAL, /**<there are more than 1 process in the circuit*/
  WAIT /**<when the process is waiting for new try connect*/
} State;

/**
* @brief state of the automaton
*/
extern State automatonState;
/**
* @brief mutex used in statemachine
*/
extern pthread_mutex_t state_machine_mutex;
/**
* @brief address of the host process
*/
extern address my_address;
/**
* @brief predecessor and successor of the host process
*/
extern address prec,succ;
/**
* @brief lists the gone and the came process
*/
extern address_set cameProc, goneProc;
/**
* @brief number of waiting
*/
extern int waitNb;
/**
* @brief last train ID sent
*/
extern int lis;
/**
* @brief last trains sent
*/
extern lts_array lts;
/**
* @brief matrix of list of unstable wagons.
*/
extern t_list* unstableWagons[MAX_NTR][NR];
/**
* @brief bqueue of wagon to deliver.
*/
extern t_bqueue* wagonsToDeliver;


/**
* @brief initializes the automaton.
*/
void automatonInit () ;


/**
* @brief handles a train when it arrived.
* @param[in] p_womim pointer of the womim to handle
*/
void train_handling(womim *p_womim) ;

/**
* @brief calculates the time to wait for WAIT state.
* @param[in] nbwait number of waiting made befor.
* @return Returns the time to wait
*/
int rand_sleep(int nbwait) ;

/**
* @brief when a connection abort the automaton closes all connexion and waits.
*/
void waitBeforConnect () ;

/**
* @brief changes state and makes initialisations that go with it.
* @param[in] s state to be assigned.
*/
void nextstate (State s) ;

/**
* @brief handles the reception of a womim depending on the state of the automaton.
* @param[in] p_womim womim to handle
*/
void stateMachine (womim* p_womim) ;

#endif
