#!/bin/sh

# init the environment by setting up C_INCLUDE_PATH
# the configuration of JAVA_HOME is not enough because some Train protocol Makefiles do not explicitly used this parameters for compile
# and this variable will be used for other applications beside this script, so export keyword is needed
# TODO: auto detection of include folders

# store init path
InitPath=`pwd`

CPath="."
JavaPath="../../../.."

echo "compile perf_InterfaceJNI.h"
cd $JavaPath/src
javac perf/InterfaceJNI.java
javah -jni perf.InterfaceJNI
cd $InitPath

cp $JavaPath/src/perf_InterfaceJNI.h $CPath/perf_InterfaceJNI.h

echo "compiling interface.o"

#TODO: works on Debian, need to check if it works for Mac
#Ref: http://en.wikipedia.org/wiki/Uname
OSName=`uname -s`

if [ "$OSName" = "Linux" ]
then
	#Compiling on Linux
	#gcc -I$JAVA_HOME/include -o libInterface.so -shared interface.c
  export C_INCLUDE_PATH=/usr/lib/jvm/java-1.7.0-openjdk-amd64/include/:/usr/lib/jvm/java-1.7.0-openjdk-amd64/include/linux:.
	make all
else
	if [ "$OSName" = "Darwin" ]
	then
		echo "compile on Mac OS X"
		#Ok if we're compiling on Mac OS X 
    export C_INCLUDE_PATH=/System/Library/Frameworks/JavaVM.framework/Headers:.
    make GCCMAKEDEP=$GCCMAKEDEP_USER all
	fi
fi

if [ ! -d $JavaPath/TrainsProtocol/lib ]
then
	mkdir $JavaPath/TrainsProtocol/lib
fi
mv libperf.* $JavaPath/TrainsProtocol/lib

