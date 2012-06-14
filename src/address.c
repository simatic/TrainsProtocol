#include <stdio.h>
#include "address.h"

address my_address;

address null_address = 0;

int addr_2_rank(address ad){
  int rank = 0;
  while (((ad & 0x0001) == 0) && (rank < MAX_MEMB)){
    ad /= 2;
    rank ++;
  }
  if (rank < MAX_MEMB)
    return rank;
  else
    return -1;
}

char *addr_2_str(char *s, address ad){
  sprintf(s, "#%02d", addr_2_rank(ad));
  return s;
}

void addr_appendArrived(address_set *arrivedSet, address ad){
  *arrivedSet |= ad;
}

void addr_appendGone(address_set *arrivedSet, address_set *goneSet, address ad){
  if (addr_ismember(ad, *arrivedSet))
    // We take out ad from arrivedSet
    *arrivedSet ^= ad;
  else
    // We add ad to goneSet
    *goneSet |= ad;
}
    
bool addr_ismember(address ad, address adSet){
  return ((ad & adSet) != 0);
}

address_set addr_updateCircuit(address_set circuit, address ad, address_set arrivedSet, address_set goneSet){
  // Notice ad is unused
  return (circuit ^ goneSet) | arrivedSet;
}

address rank_2_addr(int rank){
  return 1<<rank;
}

address addr_prec(address ad){
	int i=addr_2_rank(ad);
	if (i==(-1)) {
		error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"addr not found");
	}
	i=(i+NP-1) % NP;
	return rank_2_addr(i);
}
  

