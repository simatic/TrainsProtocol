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

#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>

#include "iomsg.h"

womim * receive(trComm * aComm){
  womim * msgExt;
  int nbRead, nbRead2;
  int length;
  int j;
  bool isPred;

  nbRead = commReadFully(aComm, &length, sizeof(length));
  if (nbRead == sizeof(length)) {
    msgExt = calloc(length + sizeof(prefix), sizeof(char));
    assert(msgExt != NULL);
    nbRead2 = commReadFully(aComm, ((char*) msgExt) + sizeof(prefix) + nbRead,
        (length - nbRead));
    if (nbRead2 == length - nbRead) {
      pthread_mutex_init(&(msgExt->pfx.mutex), NULL );
      msgExt->pfx.counter = 1;
      msgExt->msg.len = length;
      return (msgExt);
    } else {
      free(msgExt);
    }
  }

  //Connection has been closed
  //search the address which has vanished
  searchTComm(aComm, globalAddrArray, &j, &isPred);
  if (j == -1) {
    // It may happen when automaton has closed all the connections (thus
    // has erased aComm from globalAddrArray). As we are already aware of
    // this connection loss, there is nothing to do.
    return NULL ;
  }
  //create the DISCONNECT to return
  MType disconnectType;
  if (isPred) {
    disconnectType = DISCONNECT_PRED;
  } else {
    disconnectType = DISCONNECT_SUCC;
  }
  msgExt = calloc(
      sizeof(prefix) + sizeof(newMsg(disconnectType, rankToAddr(j))),
      sizeof(char));
  pthread_mutex_init(&(msgExt->pfx.mutex), NULL );
  msgExt->pfx.counter = 1;
  msgExt->msg = newMsg(disconnectType, rankToAddr(j));
  //close the connection
  closeConnection(rankToAddr(j), isPred);
  return (msgExt);
}

//Use to send all the messages Msg, even the TRAIN ones, but in fact, TRAIN messages will never be created for the sending, but use only on reception... Thus, to send TRAIN messages, sendTrain will be used.
//use globalAddrArray defined in management_addr.h
int sendOther(address addr, bool isPred, MType type, address sender){
  int length;
  int iovcnt = 1;
  struct iovec iov[1];
  int rank = -1;
  trComm * aComm;
  int result;
  Msg * msg;

  if (type == TRAIN) {
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
        "Wrong MType given to sendOther");
    return (-1);
  } else {
    msg = malloc(sizeof(newMsg(type, sender)));
    *msg = newMsg(type, sender);

    length = msg->len;
    rank = addrToRank(addr);
    if (rank != -1) {
      aComm = getTComm(rank, isPred, globalAddrArray);
      if (aComm == NULL ) {
        free(msg);
        return (-1);
      }
      //printf("Send message = %s on comm %p\n", msgTypeToStr(type), aComm);
      iov[0].iov_base = msg;
      iov[0].iov_len = length;
      result = commWritev(aComm, iov, iovcnt);
      if (result != length)
        fprintf(stderr, "result!=length\n");
      free(msg);
      return (result);
    } else {
      //should return an error if the addr is out of rank
      free(msg);
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
          "Sending failure in sendOther (addr = %d)", addr);
      return (-1);    //same error as commWritev !!
    }
  }
}

//send a train -> use sendOther to send the rest
int sendTrain(address addr, bool isPred, ltsStruct lts){
  int iovcnt = 3;
  struct iovec iov[iovcnt];
  int rank = -1;
  trComm * aComm;
  int result;

  rank = addrToRank(addr);
  if (rank != -1) {
    aComm = getTComm(rank, isPred, globalAddrArray);
    if (aComm == NULL ) {
      return (-1);
    }
    //printf("The train %d/%d is sent to %d on comm %p\n",lts.stamp.id,lts.stamp.lc,addr, aComm);

    lts.lng = sizeof(lts.lng) + sizeof(lts.type) + sizeof(lts.stamp)
        + sizeof(lts.circuit);
    //to begin, let's enter the length of the message
    iov[0].iov_base = &(lts);
    iov[0].iov_len = lts.lng;
    //after loading the wagons
    //look after to be sure there are wagons to send...
    if (lts.w.len != 0) {      //check if there are wagons
      lts.lng += lts.w.len;
      iov[1].iov_base = lts.w.w_w.p_wagon;
      iov[1].iov_len = lts.w.len;
    } else {
      iov[1].iov_base = NULL;
      iov[1].iov_len = 0;
    }
    //finally loading the wagon which is waiting to be sent
    //look after to be sure that p_wtosend exists or not...
    if (lts.p_wtosend == NULL ) {      //check if p_wtosend is NULL
      iov[2].iov_base = NULL;
      iov[2].iov_len = 0;
    } else {
      if (firstMsg(lts.p_wtosend->p_wagon) == NULL ) { //check if p_wtosend is not just a header
        printf("wagonToSend is just a header \n");
        iov[2].iov_base = NULL;
        iov[2].iov_len = 0;
      } else {
        lts.lng += lts.p_wtosend->p_wagon->header.len;
        iov[2].iov_base = lts.p_wtosend->p_wagon;
        iov[2].iov_len = lts.p_wtosend->p_wagon->header.len;
      }
    }
    //sending the whole train with writev
    //returning the number of bytes sent
    result = commWritev(aComm, iov, iovcnt);
    if (result != lts.lng)
      fprintf(stderr,
          "result!=lts.lng (bis) with result=%i and length=%i (errno = %i / %s)\n",
          result, lts.lng, errno, strerror(errno));
    return (result);
  } else {
    //should return an error if the addr is out of rank
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
        "Sending failure in sendTrain (addr = %d)", addr);
    return (-1);      //same error as commWritev !!
  }
}

