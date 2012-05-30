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

$(BIN): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *~ *.bak *.BAK *.tmp

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

# Run all the tests
run: $(BIN)
	(export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../../src; ./$(BIN) y 1 100000 1)

# Test memory problems with valgrind
valgrind: $(BIN)
	(export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../../src; valgrind $(VALGRINDFLAGS) ./$(BIN) y 1 100000 1 2>&1)

# DO NOT DELETE THIS LINE -- make depend depends on it.

basicTest.o: /usr/include/stdlib.h /usr/include/features.h
basicTest.o: /usr/include/alloca.h /usr/include/unistd.h
basicTest.o: /usr/include/getopt.h /usr/include/stdio.h /usr/include/libio.h
basicTest.o: /usr/include/_G_config.h /usr/include/wchar.h
basicTest.o: /usr/include/error.h /usr/include/errno.h
basicTest.o: /usr/include/semaphore.h /usr/include/time.h
basicTest.o: /usr/include/strings.h /usr/include/xlocale.h
basicTest.o: ../../../include/trains.h ../../../include/common.h
basicTest.o: /usr/include/pthread.h /usr/include/endian.h
basicTest.o: /usr/include/sched.h ../../../include/address.h
basicTest.o: ../../../include/applicationMessage.h
mock.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
mock.o: /usr/include/_G_config.h /usr/include/wchar.h
mock.o: ../../../include/trains.h ../../../include/common.h
mock.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/pthread.h
mock.o: /usr/include/endian.h /usr/include/sched.h /usr/include/time.h
mock.o: /usr/include/error.h ../../../include/address.h
mock.o: ../../../include/applicationMessage.h
