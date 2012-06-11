#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#include "management_addr.h"

ADDR* global_addr_array;

// Max length of a line read in LOCALISATION file
#define MAX_LEN_LINE_IN_FILE (MAX_LEN_IP + 1 + MAX_LEN_CHAN)

//create an array of addresses
ADDR* init_addr_list(int length){
  ADDR* result=calloc(length+1,sizeof(ADDR));//We calloc one more element, so that the last element of the array is an empty element
  assert(result != NULL);
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
  char line[MAX_LEN_LINE_IN_FILE];
  char * addr_full=NULL;
  char * ip_only=NULL;
  int i=0;

  addr_file = fopen (locate , "r");
  if (addr_file == NULL) error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "Error opening file");
  else {
    for(i=0;i<length;i++){
      if ( fgets (line, MAX_LEN_LINE_IN_FILE, addr_file) != NULL )
	{
	 ip_only   = strtok(line, ":");
	 addr_full = strtok(NULL, ":\n");
	 strcpy(array[i].ip,ip_only);
	 strcpy(array[i].chan,addr_full);
	}
    }
  }
  
  fclose(addr_file);

  return(array);
}

//add a tcomm to a place in an ADDR*
void add_tcomm(t_comm * tcomm, int i, ADDR * array){
  array[i].tcomm=tcomm;
}

//search a t_comm in an array
int search_tcomm(t_comm * tcomm, ADDR * array){
	int i=0;
	int result=-1;
	
	for(i=0;i<NP;i++){
		if(array[i].tcomm == tcomm)
			result=i;
	}
	return(result);
}

//give the place in the array of a given address
//return -1 if it's unfound
int addr_id(char * ip, char * chan, ADDR * array){
  int i=0;

  while((array[i].ip[0] != '\0')&&(strcmp(array[i].ip,ip)!=0 || strcmp(array[i].chan,chan)!=0)){
    i++;
  }

  if(array[i].ip[0] != '\0'){
    return(i);
  }
  else{
    return(-1);
  }
}
