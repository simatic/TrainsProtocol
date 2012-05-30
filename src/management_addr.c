#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "comm.h"

//Define the localisation of the file where addresses are written
#define LOCALISATION "./addr_file"

//structure of the file addresses variable
typedef struct addr
{
  char* ip;
  char* chan;
  t_comm* tcomm;
}ADDR;


//to create an address block with the ip, the channel and the t_comm if it exists
ADDR init_addr(void){
  ADDR addr;

  addr.ip=NULL;
  addr.chan=NULL;
  addr.tcomm=NULL;
  return(addr);
}

//create an array of addresses
ADDR* init_addr_list(int length){
  int i=0;
  ADDR* result=malloc(length*sizeof(ADDR));
  
  for(i=0;i<length;i++){
    result[i]=init_addr();
  }
  return(result);
}

//free an array of addresses
void free_addr_list(ADDR* tab){
  free(tab);
}

//read the file of addresses to fulfill our array
//it takes to arguments : the place where is the file and the maximal number of addresses it could have.
ADDR* addr_generator(char* locate, int length){
  FILE * addr_file;
  ADDR * array=init_addr_list(length);
  char * addr_full=NULL;
  char * ip_only=NULL;
  int i=0;

  addr_file = fopen (locate , "r");
  if (addr_file == NULL) perror ("Error opening file");
  else {
    for(i=0;i<length;i++){
      addr_full=NULL;
      ip_only=NULL;
      if ( fgets (addr_full , 100 , addr_file) != NULL )
	{
	 ip_only =strtok (addr_full,":");
	 memcpy(array[i].ip,ip_only,strlen(ip_only)+1);
	 memcpy(array[i].chan,addr_full,strlen(addr_full)+1);
	}
    }
  }
  
  fclose(addr_file);
  free(addr_full);
  free(ip_only);

  return(array);
}

//add a tcomm to a place in an ADDR*
void add_tcomm(t_comm * tcomm, int i, ADDR * array){
  array[i].tcomm=tcomm;
}

//give the place in the array of a given address
//return -1 if it's unfound
int addr_id(char * ip, char * chan, ADDR * array){
  int length=sizeof(array)/sizeof(ADDR);
  int i=0;

  while((i<length)&&(strcmp(array[i].ip,ip)!=0 || strcmp(array[i].chan,chan)!=0)){
    i++;
  }

  if(i<length){
    return(i);
  }
  else{
    return(-1);
  }
}
