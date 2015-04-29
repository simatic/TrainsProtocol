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

 Developer(s): Michel Simatic, Arthur Foltz, Damien Graux, Nicolas Hascoet, Stephanie Ouillon, Nathan Reboud
*/

#include "applicationMessage.h"
#include <jni.h>

extern char *theJNICallbackCircuitChange;
extern char *theJNICallbackODeliver;
extern JavaVM *jvm;

/* Callback IDs*/
extern jobject jcallbackODeliver;
extern jobject jcallbackCircuitChange;
extern jmethodID jcallbackODeliver_runID;
extern jmethodID jcallbackCircuitChange_runID;
 
/* Objects & Fields IDs*/
extern jobject jmsghdr;
extern jfieldID jmsghdr_lenID;
extern jfieldID jmsghdr_typeID;

extern jobject jmsg;
extern jfieldID jmsg_hdrID;
extern jfieldID jmsg_payloadID;

extern jobject jcv;
extern jfieldID jcv_nmembID;
extern jfieldID jcv_joinedID;
extern jfieldID jcv_departedID;
extern jmethodID jcv_setMembersAddressID;
