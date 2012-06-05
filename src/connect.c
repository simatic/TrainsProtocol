#include <stdlib.h>

#include "connect.h"

int open_connection(address addr){
  int rank;
  t_comm * tcomm;

  rank=addr_2_rank(addr);
  if (rank==-1){
    error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"Wrong address %d sent to open_connection",addr);
    return(-1);
  }
  else{
    tcomm=comm_newAndConnect(global_addr_array[rank].ip,global_addr_array[rank].chan,1000);//FIXME -> Time connect out param
    if (tcomm==NULL)
      return(-1);
    else{
      global_addr_array[rank].tcomm=tcomm;
      return(1);
    }
  }
}

void close_connection(address addr){
  int rank;
  t_comm * tcomm;
  
  rank=addr_2_rank(addr);
  if (rank==-1)
    error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"Wrong address %d sent to close_connection",addr);
  else{
    tcomm=global_addr_array[rank].tcomm;
    if(tcomm==NULL)
      error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"Wrong address sent to close_connection");
    else{
      comm_abort(tcomm);
      global_addr_array[rank].tcomm=NULL;
    }
  }           
}

address searchSucc(address add){
  int i;
  int watch=1;
  int rank;
  address result=my_address;

  rank=addr_2_rank(add);
  if(rank==-1)
    error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"Wrong address %d given to searchSucc",add);
  else{
    i=(rank+1)%NP;
    while(i!=rank && watch){
      if(open_connection(i)==1){
	result=rank_2_addr(i);
	watch=0;
      }
      else{
	i++;
      }
    }
  }
  return(result);
}
