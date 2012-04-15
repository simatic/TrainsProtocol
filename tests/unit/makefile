### Configuration.  ####################################################

# Binaries to build
BIN		= testBqueue testComm testList
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

testBqueue.o: /usr/include/stdio.h /usr/include/features.h
testBqueue.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
testBqueue.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
testBqueue.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
testBqueue.o: /usr/include/bits/typesizes.h /usr/include/libio.h
testBqueue.o: /usr/include/_G_config.h /usr/include/wchar.h
testBqueue.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
testBqueue.o: /usr/include/stdlib.h /usr/include/sys/types.h
testBqueue.o: /usr/include/time.h /usr/include/endian.h
testBqueue.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
testBqueue.o: /usr/include/sys/select.h /usr/include/bits/select.h
testBqueue.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
testBqueue.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
testBqueue.o: /usr/include/alloca.h /usr/include/pthread.h
testBqueue.o: /usr/include/sched.h /usr/include/bits/sched.h
testBqueue.o: /usr/include/signal.h /usr/include/bits/setjmp.h
testBqueue.o: /usr/include/error.h /usr/include/bits/error.h
testBqueue.o: ../../include/bqueue.h /usr/include/semaphore.h
testBqueue.o: /usr/include/bits/semaphore.h ../../include/list.h
testComm.o: /usr/include/stdio.h /usr/include/features.h
testComm.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
testComm.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
testComm.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
testComm.o: /usr/include/bits/typesizes.h /usr/include/libio.h
testComm.o: /usr/include/_G_config.h /usr/include/wchar.h
testComm.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
testComm.o: /usr/include/stdlib.h /usr/include/sys/types.h
testComm.o: /usr/include/time.h /usr/include/endian.h
testComm.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
testComm.o: /usr/include/sys/select.h /usr/include/bits/select.h
testComm.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
testComm.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
testComm.o: /usr/include/alloca.h /usr/include/pthread.h /usr/include/sched.h
testComm.o: /usr/include/bits/sched.h /usr/include/signal.h
testComm.o: /usr/include/bits/setjmp.h /usr/include/error.h
testComm.o: /usr/include/bits/error.h /usr/include/errno.h
testComm.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
testComm.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
testComm.o: /usr/include/asm-generic/errno-base.h /usr/include/unistd.h
testComm.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
testComm.o: /usr/include/getopt.h /usr/include/string.h
testComm.o: /usr/include/xlocale.h /usr/include/assert.h ../../include/comm.h
testComm.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
testComm.o: ../../include/common.h ../../include/trains.h
testList.o: /usr/include/stdio.h /usr/include/features.h
testList.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
testList.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
testList.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
testList.o: /usr/include/bits/typesizes.h /usr/include/libio.h
testList.o: /usr/include/_G_config.h /usr/include/wchar.h
testList.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
testList.o: /usr/include/stdlib.h /usr/include/sys/types.h
testList.o: /usr/include/time.h /usr/include/endian.h
testList.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
testList.o: /usr/include/sys/select.h /usr/include/bits/select.h
testList.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
testList.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
testList.o: /usr/include/alloca.h ../../include/list.h /usr/include/pthread.h
testList.o: /usr/include/sched.h /usr/include/bits/sched.h
testList.o: /usr/include/signal.h /usr/include/bits/setjmp.h
