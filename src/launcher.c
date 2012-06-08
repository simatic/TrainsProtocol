/*#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <error.h>

#include "launcher.h"

void * thread_utoDeliver

void * thread_read(void* tcomm){
  womim * womi;
  do{
    womi=receive(tcomm);
  }while(1);
  stateMachine(womi);
}

void * thread_ear(){
  t_comm * tcomm;
  pthread_t read;

  do{
    pthread_detach(pthread_self());//to allow him to live his own life
    do{  
      tcomm=comm_newForAccept(PORT);
    }while(tcomm==NULL);//FIXME -> what about the upgrading of global_addr_array..!
    pthread_create(&read,NULL,thread_read(tcomm),NULL);
  }while(participation==true);

  pthread_exit(NULL);//FIXME -> is it really usefull because of the "pthread_detach"?
}

void tr_init(fr,fc){


  my_address=rank_2_addr(addr_id(LOCAL_HOST,PORT,CONNECT_TIMEOUT));
  //automaton_init();//FIXME -> sure of it place in the code ?
  //  sem_wait();//FIXME

  //threads de michel simatic (wagon->application & application->wagon)
  //thread d'écoute sur le port PORT et qui lance le thread de la machine à états
}
*/
