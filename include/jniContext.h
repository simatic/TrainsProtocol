#include "applicationMessage.h"
#include <jni.h>

extern char *theJNICallbackCircuitChange;
extern char *theJNICallbackUtoDeliver;
extern JavaVM *jvm;

/* Callback IDs*/
extern jobject jcallbackUtoDeliver;
extern jobject jcallbackCircuitChange;
extern jmethodID jcallbackUtoDeliver_runID;
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
extern jfieldID jcv_membersID;
extern jfieldID jcv_joinedID;
extern jfieldID jcv_departedID;
