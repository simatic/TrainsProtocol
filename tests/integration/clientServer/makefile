### Configuration.  ####################################################

# Binaries to build
BIN		= client server
# C source files for the binaries
SOURCES		= $(wildcard *.c)
# Object files corresponding to source files
OBJECTS		= $(SOURCES:.c=.o)
# Include directory
INCLUDEDIRS = -I../../../include
# Compilation flags
CFLAGS = -Wall -g $(INCLUDEDIRS)
# Link flags
LDFLAGS = -L../../../src -ltrains -pthread
# Valgrind options
VALGRINDFLAGS =--leak-check=yes --leak-resolution=high --num-callers=40 --tool=memcheck --show-reachable=yes

### Rules.  ############################################################

.PHONY:         all clean cleanall

all: depend $(BIN)

client: client.o
	$(CC) $^ -o $@ $(LDFLAGS)

server: server.o
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *~ *.bak *.BAK *.tmp

cleanall: clean
	rm -f $(OBJECTS) $(BIN) depend

depend: $(SOURCES)
	touch depend
	makedepend -- $(CFLAGS) -- $(SOURCES) 2>/dev/null

# Run all the tests
run: $(BIN)
	(export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../../src; ./basicTest y 1 100000 1)

# Test memory problems with valgrind
valgrind: $(BIN)
	(export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../../src; valgrind $(VALGRINDFLAGS) ./basicTest y 1 100000 1 2>&1)

# DO NOT DELETE THIS LINE -- make depend depends on it.

client.o: /usr/include/stdio.h /usr/include/features.h
client.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
client.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
client.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
client.o: /usr/include/bits/typesizes.h /usr/include/libio.h
client.o: /usr/include/_G_config.h /usr/include/wchar.h
client.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
client.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
client.o: /usr/include/endian.h /usr/include/bits/endian.h
client.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
client.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
client.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
client.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
client.o: /usr/include/pthread.h /usr/include/sched.h
client.o: /usr/include/bits/sched.h /usr/include/signal.h
client.o: /usr/include/bits/setjmp.h /usr/include/error.h
client.o: /usr/include/bits/error.h /usr/include/errno.h
client.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
client.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
client.o: /usr/include/asm-generic/errno-base.h /usr/include/unistd.h
client.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
client.o: /usr/include/getopt.h /usr/include/string.h /usr/include/xlocale.h
client.o: /usr/include/assert.h ../../../include/comm.h
client.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
client.o: ../../../include/common.h ../../../include/trains.h
server.o: /usr/include/stdio.h /usr/include/features.h
server.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
server.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
server.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
server.o: /usr/include/bits/typesizes.h /usr/include/libio.h
server.o: /usr/include/_G_config.h /usr/include/wchar.h
server.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
server.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
server.o: /usr/include/endian.h /usr/include/bits/endian.h
server.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
server.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
server.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
server.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
server.o: /usr/include/pthread.h /usr/include/sched.h
server.o: /usr/include/bits/sched.h /usr/include/signal.h
server.o: /usr/include/bits/setjmp.h /usr/include/error.h
server.o: /usr/include/bits/error.h /usr/include/errno.h
server.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
server.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
server.o: /usr/include/asm-generic/errno-base.h /usr/include/unistd.h
server.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
server.o: /usr/include/getopt.h /usr/include/string.h /usr/include/xlocale.h
server.o: /usr/include/assert.h ../../../include/comm.h
server.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
server.o: ../../../include/common.h ../../../include/trains.h
