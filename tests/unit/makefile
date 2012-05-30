### Configuration.  ####################################################

# Binaries to build
BIN		= testAddress testApplicationMessage_wagon testBqueue testComm testList
# C source files for the binaries
SOURCES		= $(wildcard *.c)
# Include directory
INCLUDEDIRS = -I../../include
# Compilation flags
CFLAGS = -g -Wall $(INCLUDEDIRS)
# Link flags
LDFLAGS = -g -L../../src -ltrains -pthread
# Valgrind options
VALGRINDFLAGS =--leak-check=yes --leak-resolution=high --num-callers=40 --tool=memcheck --show-reachable=yes

### Rules.  ############################################################

.PHONY:         all clean cleanall run valgrind

all: depend $(BIN)

testAddress: testAddress.o
	$(CC) $^ -o $@ $(LDFLAGS)

testApplicationMessage_wagon: testApplicationMessage_wagon.o
	$(CC) $^ -o $@ $(LDFLAGS)

testBqueue: testBqueue.o
	$(CC) $^ -o $@ $(LDFLAGS)

testComm: testComm.o
	$(CC) $^ -o $@ $(LDFLAGS)

testList: testList.o
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *~ *.bak *.BAK 

cleanall: clean
	rm -f *.o $(BIN) depend
	# We cut lines after "# DO NOT DELETE THIS LINE" so that
	# we do not keep in GIT dependencies related to a given distribution
	# (e.g. Ubuntu 10.04) which are not compatible with other distributions
	# (e.g. Ubuntu 11.10)
	sed '/^# DO NOT DELETE THIS LINE -- make depend depends on it./q' makefile >makefile.tmp
	mv makefile.tmp makefile

depend: $(SOURCES)
	touch depend
	makedepend -- $(CFLAGS) -- $(SOURCES) 2>/dev/null

# Run all the tests
run: $(BIN)
	for i in $(BIN); do (echo Running $$i; export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../src; ./$$i); done

# Test memory problems with valgrind
valgrind: $(BIN)
	for i in $(BIN); do (echo Running $$i; export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../src; valgrind $(VALGRINDFLAGS) ./$$i 2>&1); done

# DO NOT DELETE THIS LINE -- make depend depends on it.

testAddress.o: /usr/include/stdio.h /usr/include/features.h
testAddress.o: /usr/include/libio.h /usr/include/_G_config.h
testAddress.o: /usr/include/wchar.h /usr/include/stdlib.h
testAddress.o: /usr/include/alloca.h /usr/include/string.h
testAddress.o: /usr/include/xlocale.h ../../include/address.h
testAddress.o: ../../include/common.h /usr/include/pthread.h
testAddress.o: /usr/include/endian.h /usr/include/sched.h /usr/include/time.h
testAddress.o: /usr/include/error.h
testApplicationMessage_wagon.o: /usr/include/stdio.h /usr/include/features.h
testApplicationMessage_wagon.o: /usr/include/libio.h /usr/include/_G_config.h
testApplicationMessage_wagon.o: /usr/include/wchar.h /usr/include/stdlib.h
testApplicationMessage_wagon.o: /usr/include/alloca.h /usr/include/string.h
testApplicationMessage_wagon.o: /usr/include/xlocale.h /usr/include/assert.h
testApplicationMessage_wagon.o: /usr/include/unistd.h /usr/include/getopt.h
testApplicationMessage_wagon.o: ../../include/common.h /usr/include/pthread.h
testApplicationMessage_wagon.o: /usr/include/endian.h /usr/include/sched.h
testApplicationMessage_wagon.o: /usr/include/time.h /usr/include/error.h
testApplicationMessage_wagon.o: ../../include/applicationMessage.h
testApplicationMessage_wagon.o: ../../include/address.h ../../include/wagon.h
testApplicationMessage_wagon.o: ../../include/bqueue.h
testApplicationMessage_wagon.o: /usr/include/semaphore.h ../../include/list.h
testApplicationMessage_wagon.o: ../../include/trains.h
testBqueue.o: /usr/include/stdio.h /usr/include/features.h
testBqueue.o: /usr/include/libio.h /usr/include/_G_config.h
testBqueue.o: /usr/include/wchar.h /usr/include/stdlib.h
testBqueue.o: /usr/include/alloca.h /usr/include/pthread.h
testBqueue.o: /usr/include/endian.h /usr/include/sched.h /usr/include/time.h
testBqueue.o: /usr/include/error.h ../../include/bqueue.h
testBqueue.o: /usr/include/semaphore.h ../../include/list.h
testComm.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
testComm.o: /usr/include/_G_config.h /usr/include/wchar.h
testComm.o: /usr/include/stdlib.h /usr/include/alloca.h
testComm.o: /usr/include/pthread.h /usr/include/endian.h /usr/include/sched.h
testComm.o: /usr/include/time.h /usr/include/error.h /usr/include/errno.h
testComm.o: /usr/include/unistd.h /usr/include/getopt.h /usr/include/string.h
testComm.o: /usr/include/xlocale.h /usr/include/assert.h ../../include/comm.h
testComm.o: ../../include/common.h ../../include/trains.h
testComm.o: ../../include/address.h ../../include/applicationMessage.h
testList.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
testList.o: /usr/include/_G_config.h /usr/include/wchar.h
testList.o: /usr/include/stdlib.h /usr/include/alloca.h ../../include/list.h
testList.o: /usr/include/pthread.h /usr/include/endian.h /usr/include/sched.h
testList.o: /usr/include/time.h
