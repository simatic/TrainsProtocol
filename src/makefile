### Configuration.  ####################################################

# Binary to build
BIN		= libtrains.so
# C source files for the library
SOURCES		= $(wildcard *.c)
# Object files corresponding to source files
OBJECTS		= $(SOURCES:.c=.o)
# Include directory
INCLUDEDIRS = -I../include
# Compilation flags (the -fPIC flag is because the files are compiled for a dynamic library)
CFLAGS = -g -Wall $(INCLUDEDIRS) -fPIC 
# Link flags
LDFLAGS = -g -shared

### Rules.  ############################################################

.PHONY:         all clean cleanall

all: depend $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ 

clean:
	rm -f *~ *.bak *.BAK 

cleanall: clean
	rm -f $(OBJECTS) $(BIN) depend
	# We cut lines after "# DO NOT DELETE THIS LINE" so that
	# we do not keep in GIT dependencies related to a given distribution
	# (e.g. Ubuntu 10.04) which are not compatible with other distributions
	# (e.g. Ubuntu 11.10)
	sed '/^# DO NOT DELETE THIS LINE -- make depend depends on it./q' makefile >makefile.tmp
	mv makefile.tmp makefile

depend: $(SOURCES)
	touch depend
	makedepend -- $(CFLAGS) -- $(SOURCES) 2>/dev/null

# DO NOT DELETE THIS LINE -- make depend depends on it.

address.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
address.o: /usr/include/_G_config.h /usr/include/wchar.h ../include/address.h
address.o: ../include/common.h /usr/include/stdlib.h /usr/include/alloca.h
address.o: /usr/include/pthread.h /usr/include/endian.h /usr/include/sched.h
address.o: /usr/include/time.h /usr/include/error.h
applicationMessage.o: /usr/include/string.h /usr/include/features.h
applicationMessage.o: /usr/include/xlocale.h /usr/include/stdio.h
applicationMessage.o: /usr/include/libio.h /usr/include/_G_config.h
applicationMessage.o: /usr/include/wchar.h ../include/applicationMessage.h
applicationMessage.o: ../include/address.h ../include/common.h
applicationMessage.o: /usr/include/stdlib.h /usr/include/alloca.h
applicationMessage.o: /usr/include/pthread.h /usr/include/endian.h
applicationMessage.o: /usr/include/sched.h /usr/include/time.h
applicationMessage.o: /usr/include/error.h ../include/trains.h
applicationMessage.o: ../include/wagon.h ../include/bqueue.h
applicationMessage.o: /usr/include/semaphore.h ../include/list.h
applicationMessage.o: ../include/counter.h
bqueue.o: /usr/include/error.h /usr/include/features.h /usr/include/errno.h
bqueue.o: /usr/include/assert.h /usr/include/stdlib.h /usr/include/alloca.h
bqueue.o: ../include/bqueue.h /usr/include/semaphore.h /usr/include/time.h
bqueue.o: ../include/list.h /usr/include/pthread.h /usr/include/endian.h
bqueue.o: /usr/include/sched.h
comm.o: /usr/include/error.h /usr/include/features.h /usr/include/assert.h
comm.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/unistd.h
comm.o: /usr/include/getopt.h /usr/include/string.h /usr/include/xlocale.h
comm.o: /usr/include/errno.h /usr/include/netdb.h /usr/include/netinet/in.h
comm.o: /usr/include/stdint.h /usr/include/endian.h /usr/include/rpc/netdb.h
comm.o: /usr/include/netinet/tcp.h /usr/include/stdio.h /usr/include/libio.h
comm.o: /usr/include/_G_config.h /usr/include/wchar.h ../include/comm.h
comm.o: /usr/include/pthread.h /usr/include/sched.h /usr/include/time.h
comm.o: ../include/common.h ../include/signalMgt.h /usr/include/signal.h
counter.o: ../include/counter.h
list.o: /usr/include/error.h /usr/include/features.h /usr/include/assert.h
list.o: /usr/include/stdlib.h /usr/include/alloca.h ../include/list.h
list.o: /usr/include/pthread.h /usr/include/endian.h /usr/include/sched.h
list.o: /usr/include/time.h ../include/common.h
management_addr.o: /usr/include/stdlib.h /usr/include/features.h
management_addr.o: /usr/include/alloca.h /usr/include/string.h
management_addr.o: /usr/include/xlocale.h /usr/include/stdio.h
management_addr.o: /usr/include/libio.h /usr/include/_G_config.h
management_addr.o: /usr/include/wchar.h ../include/comm.h
management_addr.o: /usr/include/pthread.h /usr/include/endian.h
management_addr.o: /usr/include/sched.h /usr/include/time.h
management_addr.o: ../include/common.h /usr/include/error.h
msg.o: /usr/include/stdlib.h /usr/include/features.h /usr/include/alloca.h
msg.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
msg.o: /usr/include/wchar.h ../include/address.h ../include/common.h
msg.o: /usr/include/pthread.h /usr/include/endian.h /usr/include/sched.h
msg.o: /usr/include/time.h /usr/include/error.h ../include/msg.h
msg.o: ../include/str_train.h ../include/wagon.h
msg.o: ../include/applicationMessage.h ../include/bqueue.h
msg.o: /usr/include/semaphore.h ../include/list.h
signalMgt.o: /usr/include/stdio.h /usr/include/features.h
signalMgt.o: /usr/include/libio.h /usr/include/_G_config.h
signalMgt.o: /usr/include/wchar.h /usr/include/stdlib.h /usr/include/alloca.h
signalMgt.o: /usr/include/error.h /usr/include/errno.h /usr/include/pthread.h
signalMgt.o: /usr/include/endian.h /usr/include/sched.h /usr/include/time.h
signalMgt.o: /usr/include/unistd.h /usr/include/getopt.h
signalMgt.o: ../include/signalMgt.h /usr/include/signal.h ../include/common.h
str_train.o: ../include/str_train.h /usr/include/pthread.h
str_train.o: /usr/include/features.h /usr/include/endian.h
str_train.o: /usr/include/sched.h /usr/include/time.h ../include/msg.h
str_train.o: ../include/address.h ../include/common.h /usr/include/stdlib.h
str_train.o: /usr/include/alloca.h /usr/include/error.h ../include/wagon.h
str_train.o: ../include/applicationMessage.h ../include/bqueue.h
str_train.o: /usr/include/semaphore.h ../include/list.h
wagon.o: /usr/include/stdlib.h /usr/include/features.h /usr/include/alloca.h
wagon.o: /usr/include/pthread.h /usr/include/endian.h /usr/include/sched.h
wagon.o: /usr/include/time.h /usr/include/assert.h ../include/bqueue.h
wagon.o: /usr/include/semaphore.h ../include/list.h ../include/wagon.h
wagon.o: ../include/address.h ../include/common.h /usr/include/error.h
wagon.o: ../include/applicationMessage.h
