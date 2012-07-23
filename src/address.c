/**
 Trains Protocol: Middleware for Uniform and Totally Ordered Broadcasts
 Copyright: Copyright (C) 2010-2012
 Contact: michel.simatic@telecom-sudparis.eu

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA

 Developer(s): Michel Simatic, Arthur Foltz, Damien Graux, Nicolas Hascoet, Nathan Reboud
 */

#include <stdio.h>
#include "address.h"

address myAddress;

address nullAddress = 0;

int addrToRank(address ad){
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

char *addrToStr(char *s, address ad){
  sprintf(s, "#%02d", addrToRank(ad));
  return s;
}

void addrAppendArrived(addressSet *arrivedSet, address ad){
  *arrivedSet |= ad;
}

void addrAppendGone(addressSet *arrivedSet, addressSet *goneSet, address ad){
  if (addrIsMember(ad, *arrivedSet))
    // We take out ad from arrivedSet
    *arrivedSet ^= ad;
  else
    // We add ad to goneSet
    *goneSet |= ad;
}
    
bool addrIsMember(address ad, address adSet){
  return ((ad & adSet) != 0);
}

addressSet addrUpdateCircuit(addressSet circuit, address ad, addressSet arrivedSet, addressSet goneSet){
  // Notice ad is unused
  return (circuit ^ goneSet) | arrivedSet;
}

address rankToAddr(int rank){
  return 1<<rank;
}

address addrPrec(address ad, addressSet circuit){
  int i=addrToRank(ad);
  if (i==(-1)) {
    error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"addr not found");
  }
  do {
    i=(i+NP-1) % NP;
  } while (!addrIsMember(rankToAddr(i), circuit));
  return rankToAddr(i);
}
  

