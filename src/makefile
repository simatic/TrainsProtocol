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
CFLAGS = -Wall $(INCLUDEDIRS) -fPIC 
# Link flags
LDFLAGS = -shared

### Rules.  ############################################################

.PHONY:         all clean cleanall

all: depend $(BIN)

libtrains.so: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ 

clean:
	rm -f *~ *.bak *.BAK 

cleanall: clean
	rm -f $(OBJECTS) $(BIN) depend

depend: $(SOURCES)
	touch depend
	makedepend -- $(CFLAGS) -- $(SOURCES) 2>/dev/null

# DO NOT DELETE THIS LINE -- make depend depends on it.

bqueue.o: /usr/include/error.h /usr/include/features.h
bqueue.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
bqueue.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
bqueue.o: /usr/include/gnu/stubs-32.h /usr/include/bits/error.h
bqueue.o: /usr/include/errno.h /usr/include/bits/errno.h
bqueue.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
bqueue.o: /usr/include/asm-generic/errno.h
bqueue.o: /usr/include/asm-generic/errno-base.h /usr/include/assert.h
bqueue.o: /usr/include/stdlib.h /usr/include/sys/types.h
bqueue.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
bqueue.o: /usr/include/time.h /usr/include/endian.h
bqueue.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
bqueue.o: /usr/include/sys/select.h /usr/include/bits/select.h
bqueue.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
bqueue.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
bqueue.o: /usr/include/alloca.h ../include/bqueue.h /usr/include/semaphore.h
bqueue.o: /usr/include/bits/semaphore.h ../include/list.h
bqueue.o: /usr/include/pthread.h /usr/include/sched.h
bqueue.o: /usr/include/bits/sched.h /usr/include/signal.h
bqueue.o: /usr/include/bits/setjmp.h
list.o: /usr/include/error.h /usr/include/features.h
list.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
list.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
list.o: /usr/include/gnu/stubs-32.h /usr/include/bits/error.h
list.o: /usr/include/assert.h /usr/include/stdlib.h /usr/include/sys/types.h
list.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
list.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
list.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
list.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
list.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
list.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
list.o: ../include/list.h /usr/include/pthread.h /usr/include/sched.h
list.o: /usr/include/bits/sched.h /usr/include/signal.h
list.o: /usr/include/bits/setjmp.h
