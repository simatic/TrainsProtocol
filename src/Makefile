### Configuration.  ####################################################


UNAME := $(shell uname)

# Binaries to build
STATIC_LIB = ../lib/libtrains.a
ifeq ($(UNAME), Linux)
	RANLIB_OPTIONS =  
	DYNAMIC_LIB  = ../lib/libtrains.so
	JNI_LIB = ../lib/phonyLibWhichIsHereBecauseWeNeedAJNI_LINOnMacOS.phonyLib
endif
ifeq ($(UNAME), Darwin)
	# On MacOS, we need -c option (http://lists.gnu.org/archive/html/libtool/2002-07/msg00025.html) and this option does not exist on Linux
	RANLIB_OPTIONS = -c
	DYNAMIC_LIB  = ../lib/libtrains.dylib
	JNI_LIB  = ../lib/libtrains.jnilib
endif
BIN = $(STATIC_LIB) $(DYNAMIC_LIB) $(JNI_LIB)
# C source files
SOURCES  = $(wildcard *.c)
# Object files of static library
OBJECTS_STATIC  = $(SOURCES:.c=.o)
# Object files of dynamic library
OBJECTS_DYNAMIC  = address_dyn.o advanced_struct_dyn.o applicationMessage_dyn.o bqueue_dyn.o comm_dyn.o connect_dyn.o counter_dyn.o interface_dyn.o iomsg_dyn.o list_dyn.o management_addr_dyn.o msg_dyn.o signalArrival_dyn.o stateMachine_dyn.o wagon_dyn.o
# Include directory
INCLUDEDIRS = -I../include -I$(JAVA_HOME)/include
# Common compilation flags
COMMON_CFLAGS = -g -Wall $(INCLUDEDIRS) $(GLOBALFLAGS) -pthread
# Common link flags
COMMON_LDFLAGS = -g

### Rules.  ############################################################

.PHONY:         all allForJNI allWithInstrumentation clean

all: depend $(BIN)

allForJNI: GLOBALFLAGS = -DJNI
allForJNI: depend $(BIN)

allWithInstrumentation: GLOBALFLAGS = -DINSERTION_TEST
allWithInstrumentation: depend $(BIN)

$(STATIC_LIB): CFLAGS = $(COMMON_CFLAGS)
$(STATIC_LIB): $(OBJECTS_STATIC)
	mkdir -p ../lib
	ar rc $@ $^
	ranlib $(RANLIB_OPTIONS) $@

$(DYNAMIC_LIB): CFLAGS = $(COMMON_CFLAGS) -fPIC
$(DYNAMIC_LIB): $(OBJECTS_DYNAMIC)
	mkdir -p ../lib
	$(CC) $(COMMON_LDFLAGS) -shared $^ -o $@ 

$(JNI_LIB): $(DYNAMIC_LIB)
	ln -s $(DYNAMIC_LIB) $(JNI_LIB)

address_dyn.o: address.c
	$(CC) $(CFLAGS) $^  -c -o $@

advanced_struct_dyn.o: advanced_struct.c
	$(CC) $(CFLAGS) $^  -c -o $@

applicationMessage_dyn.o: applicationMessage.c
	$(CC) $(CFLAGS) $^  -c -o $@

bqueue_dyn.o: bqueue.c
	$(CC) $(CFLAGS) $^  -c -o $@

comm_dyn.o: comm.c
	$(CC) $(CFLAGS) $^  -c -o $@

connect_dyn.o: connect.c
	$(CC) $(CFLAGS) $^  -c -o $@

counter_dyn.o: counter.c
	$(CC) $(CFLAGS) $^  -c -o $@

interface_dyn.o: interface.c
	$(CC) $(CFLAGS) $^  -c -o $@

iomsg_dyn.o: iomsg.c
	$(CC) $(CFLAGS) $^  -c -o $@

list_dyn.o: list.c
	$(CC) $(CFLAGS) $^  -c -o $@

management_addr_dyn.o: management_addr.c
	$(CC) $(CFLAGS) $^  -c -o $@

msg_dyn.o: msg.c
	$(CC) $(CFLAGS) $^  -c -o $@

signalArrival_dyn.o: signalArrival.c
	$(CC) $(CFLAGS) $^  -c -o $@

stateMachine_dyn.o: stateMachine.c
	$(CC) $(CFLAGS) $^  -c -o $@

wagon_dyn.o: wagon.c
	$(CC) $(CFLAGS) $^  -c -o $@

clean:
	rm -f *~ *.bak *.BAK 
	rm -f $(OBJECTS_STATIC) $(OBJECTS_DYNAMIC) $(BIN) depend

depend: CFLAGS = $(COMMON_CFLAGS)
depend: $(SOURCES)
	gcc -M $(CFLAGS) $(SOURCES) >depend 2>/dev/null

-include depend
