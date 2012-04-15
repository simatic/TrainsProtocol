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
comm.o: /usr/include/error.h /usr/include/features.h
comm.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
comm.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
comm.o: /usr/include/gnu/stubs-32.h /usr/include/bits/error.h
comm.o: /usr/include/assert.h /usr/include/stdlib.h /usr/include/sys/types.h
comm.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
comm.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
comm.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
comm.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
comm.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
comm.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
comm.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
comm.o: /usr/include/bits/confname.h /usr/include/getopt.h
comm.o: /usr/include/string.h /usr/include/xlocale.h /usr/include/errno.h
comm.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
comm.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
comm.o: /usr/include/asm-generic/errno-base.h /usr/include/sys/socket.h
comm.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
comm.o: /usr/include/bits/socket.h /usr/include/bits/sockaddr.h
comm.o: /usr/include/asm/socket.h /usr/include/asm-generic/socket.h
comm.o: /usr/include/asm/sockios.h /usr/include/asm-generic/sockios.h
comm.o: /usr/include/netdb.h /usr/include/netinet/in.h /usr/include/stdint.h
comm.o: /usr/include/bits/wchar.h /usr/include/bits/in.h
comm.o: /usr/include/rpc/netdb.h /usr/include/bits/netdb.h
comm.o: /usr/include/netinet/tcp.h /usr/include/sys/time.h
comm.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
comm.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
comm.o: /usr/include/bits/sys_errlist.h ../include/comm.h
comm.o: /usr/include/pthread.h /usr/include/sched.h /usr/include/bits/sched.h
comm.o: /usr/include/signal.h /usr/include/bits/setjmp.h ../include/common.h
comm.o: ../include/signalMgt.h
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
list.o: /usr/include/bits/setjmp.h ../include/common.h
signalMgt.o: /usr/include/stdio.h /usr/include/features.h
signalMgt.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
signalMgt.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
signalMgt.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
signalMgt.o: /usr/include/bits/typesizes.h /usr/include/libio.h
signalMgt.o: /usr/include/_G_config.h /usr/include/wchar.h
signalMgt.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
signalMgt.o: /usr/include/stdlib.h /usr/include/sys/types.h
signalMgt.o: /usr/include/time.h /usr/include/endian.h
signalMgt.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
signalMgt.o: /usr/include/sys/select.h /usr/include/bits/select.h
signalMgt.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
signalMgt.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
signalMgt.o: /usr/include/alloca.h /usr/include/error.h
signalMgt.o: /usr/include/bits/error.h /usr/include/errno.h
signalMgt.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
signalMgt.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
signalMgt.o: /usr/include/asm-generic/errno-base.h /usr/include/pthread.h
signalMgt.o: /usr/include/sched.h /usr/include/bits/sched.h
signalMgt.o: /usr/include/signal.h /usr/include/bits/setjmp.h
signalMgt.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
signalMgt.o: /usr/include/bits/confname.h /usr/include/getopt.h
signalMgt.o: /usr/include/sys/syscall.h /usr/include/asm/unistd.h
signalMgt.o: /usr/include/asm/unistd_32.h /usr/include/bits/syscall.h
signalMgt.o: ../include/signalMgt.h ../include/common.h
