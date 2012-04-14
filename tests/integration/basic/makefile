### Configuration.  ####################################################

# Binaries to build
BIN		= basicTest
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

.PHONY:         all clean cleanall run valgrind

all: depend $(BIN)

basicTest: $(OBJECTS)
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
	(export LD_LIBRARY_PATH=LD_LIBRARY_PATH';'../../../src; ./basicTest y 1 100000 1)

# Test memory problems with valgrind
valgrind: $(BIN)
	(export LD_LIBRARY_PATH=LD_LIBRARY_PATH';'../../../src; valgrind $(VALGRINDFLAGS) ./basicTest y 1 100000 1 2>&1)

# DO NOT DELETE THIS LINE -- make depend depends on it.

basicTest.o: /usr/include/stdlib.h /usr/include/features.h
basicTest.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
basicTest.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
basicTest.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
basicTest.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
basicTest.o: /usr/include/time.h /usr/include/endian.h
basicTest.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
basicTest.o: /usr/include/sys/select.h /usr/include/bits/select.h
basicTest.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
basicTest.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
basicTest.o: /usr/include/alloca.h /usr/include/unistd.h
basicTest.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
basicTest.o: /usr/include/getopt.h /usr/include/stdio.h /usr/include/libio.h
basicTest.o: /usr/include/_G_config.h /usr/include/wchar.h
basicTest.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
basicTest.o: /usr/include/error.h /usr/include/bits/error.h
basicTest.o: /usr/include/errno.h /usr/include/bits/errno.h
basicTest.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
basicTest.o: /usr/include/asm-generic/errno.h
basicTest.o: /usr/include/asm-generic/errno-base.h /usr/include/semaphore.h
basicTest.o: /usr/include/bits/semaphore.h /usr/include/strings.h
basicTest.o: /usr/include/xlocale.h ../../../include/trains.h
mock.o: /usr/include/stdio.h /usr/include/features.h
mock.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
mock.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
mock.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
mock.o: /usr/include/bits/typesizes.h /usr/include/libio.h
mock.o: /usr/include/_G_config.h /usr/include/wchar.h
mock.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
mock.o: ../../../include/trains.h
