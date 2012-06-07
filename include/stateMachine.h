/**
 * @brief This file contains the state machine
 * @file stateMachine.h
 * @author Nathan REBOUD
 * @date  05/06/2012
 */
#ifndef _STATEMACHINE_H 
#define _STATEMACHINE_H

#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <unistd.h> //pour la fonction sleep
#include <time.h> //pour le rand
#include <math.h> // utiliser -lm à la compilation !
#include "address.h"
#include "iomsg.h"
#include "wagon.h"
#include "common.h"
#include "msg.h"
#include "signalArrival.h"

#define NR 3
#define WAIT_NB_MAX 10
#define WAIT_TIME 1000 //correspond au temps d'attende en microseconde
//#define ntr 3 done




/**
 * @brief Data structure for State
 */ 
typedef enum {
  OFFLINE_CONNECTION_ATTEMPT, /**<refers to the out of cirucit state when the process aims to connect*/
  OFFLINE_CONFIRMATION_WAIT,   /**<the process is waiting for a train which proves that it is in the circuit*/
  ALONE_INSERT_WAIT,	   /**<when the process is alone in the circuit*/
  ALONE_CONNECTION_WAIT, /**<the proc is alone and recieve an Insert*/
  SEVERAL,    /**<there are more than 1 process in the circuit*/
  WAIT      /**<when the process is waiting for new try connect*/
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
extern t_list* unstableWagons[ntr][NR]; 
/**
 * @brief bqueue of wagon to deliver.
 */
extern t_bqueue* wagonsToDeliver;


// extern wiw* wagonToSend : done in msg

extern bool participation; // FIXME : to erase 



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
 * @brief close all connexion and wait.
 */
void waitAlgo () ;
	
/**
 * @brief changes state and makes initialisations that go with it.
 * @param[in] s state to be assigned.
 */
void nextstate (State s) ;

/**
 * @brief handles the reception of a womim depending on the state of the automaton.
 * @param[in] p_womim womim to handle
 */	
int stateMachine (womim* p_womim) ;

#endif
