#include "applicationMessage.h"
#include <jni.h>

extern char *theJNICallbackCircuitChange;
extern char *theJNICallbackUtoDeliver;
extern JavaVM *jvm;

/* Callbacks */
//extern jmethodID jcircuitChangeId;
//extern jmethodID jutoDeliverId;
 
jobject jmsghdr;
jfieldID jmsghdr_lenID;
jfieldID jmsghdr_typeID;

