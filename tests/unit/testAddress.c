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
#include <stdlib.h>
#include <string.h>

#include "address.h"

address myAddress = 0x0001;
address nullAddress = 0x0000;
char s[MAX_LEN_ADDRESS_AS_STR];

void compare(char *testType, int ad, int target){
  printf("Testing %s...", testType);
  if (ad != target){
    printf("... KO (ad is \"%d\" instead of \"%d\")\n", ad, target);
    exit(EXIT_FAILURE);
  }
  printf("...OK\n");
}

int main() {
  addressSet arrivedSet;
  addressSet goneSet;

  compare("addrIsNull", addrIsNull(nullAddress), true);
  compare("addrIsNull", addrIsNull(myAddress), false);

  compare("addrCmp", addrCmp(nullAddress,myAddress), true);
  compare("addrCmp", addrCmp(myAddress,nullAddress), false);

  compare("addrIsEqual", addrIsEqual(nullAddress,nullAddress), true);
  compare("addrIsEqual", addrIsEqual(myAddress,nullAddress), false);

  compare("addrIsMine", addrIsMine(myAddress), true);
  compare("addrIsMine", addrIsMine(nullAddress), false);

  printf("Testing %s...", "addr_2_str");
  addrToStr(s,myAddress);
  if (strcmp(s, "#00") != 0){
    printf("... KO (s is \"%s\" instead of \"%s\")\n", s, "#00");
    exit(EXIT_FAILURE);
  }
  printf("...OK\n");

  compare("addrToRank", addrToRank(myAddress), 0);
  compare("addrToRank", addrToRank(0x8000), 15);
  compare("addrToRank", addrToRank(nullAddress), -1);

  compare("rankToAddr", rankToAddr(0), myAddress);
  compare("rankToAddr", rankToAddr(15), 0x8000);
  compare("rankToAddr", rankToAddr(16), nullAddress);

  compare("addrIsMember", addrIsMember(myAddress, 0xFFFF), true);
  compare("addrIsMember", addrIsMember(myAddress, 0xFFFE), false);

  arrivedSet = 0xFFFE;
  addrAppendArrived(&arrivedSet, myAddress);
  compare("addrAppendArrived", arrivedSet, 0xFFFF);
  arrivedSet = 0x0000;
  addrAppendArrived(&arrivedSet, myAddress);
  compare("addrAppendArrived", arrivedSet, 0x0001);

  arrivedSet = 0x0000;
  goneSet = 0xFFFE;
  addrAppendGone(&arrivedSet, &goneSet, myAddress);
  printf("Testing %s...", "addr_appendGone");
  if ((arrivedSet != 0x0000) || (goneSet != 0xFFFF)){
    printf("... KO (arrivedSet is \"%d\" instead of \"%d\" OR goneSet is \"%d\" instead of \"%d\")\n", arrivedSet, 0x0000, goneSet, 0xFFFF);
    exit(EXIT_FAILURE);
  }
  printf("...OK\n");
  arrivedSet = 0xFFFF;
  goneSet = 0x0000;
  addrAppendGone(&arrivedSet, &goneSet, myAddress);
  printf("Testing %s...", "addr_appendGone");
  if ((arrivedSet != 0xFFFE) || (goneSet != 0x0000)){
    printf("... KO (arrivedSet is \"%d\" instead of \"%d\" OR goneSet is \"%d\" instead of \"%d\")\n", arrivedSet, 0xFFFF, goneSet, 0x0000);
    exit(EXIT_FAILURE);
  }
  printf("...OK\n");

  compare("addrUpdateCircuit", addrUpdateCircuit(0xFFFF, myAddress, 0xFF00, 0x00FF), 0xFF00);

  compare("addrPrec", addrPrec(0x8000, 0x8000|0x0001), 0x0001);
  compare("addrPrec", addrPrec(0x0001, 0x8000|0x0001), 0x8000);
  return EXIT_SUCCESS;
}

  
