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
	(export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../../src; ./basicTest y 1 100000 1)

# Test memory problems with valgrind
valgrind: $(BIN)
	(export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../../src; valgrind $(VALGRINDFLAGS) ./basicTest y 1 100000 1 2>&1)

# DO NOT DELETE THIS LINE -- make depend depends on it.

client.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
client.o: /usr/include/_G_config.h /usr/include/wchar.h /usr/include/stdlib.h
client.o: /usr/include/alloca.h /usr/include/pthread.h /usr/include/endian.h
client.o: /usr/include/sched.h /usr/include/time.h /usr/include/error.h
client.o: /usr/include/errno.h /usr/include/unistd.h /usr/include/getopt.h
client.o: /usr/include/string.h /usr/include/xlocale.h /usr/include/assert.h
client.o: ../../../include/comm.h ../../../include/common.h
client.o: ../../../include/trains.h ../../../include/address.h
client.o: ../../../include/applicationMessage.h
server.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
server.o: /usr/include/_G_config.h /usr/include/wchar.h /usr/include/stdlib.h
server.o: /usr/include/alloca.h /usr/include/pthread.h /usr/include/endian.h
server.o: /usr/include/sched.h /usr/include/time.h /usr/include/error.h
server.o: /usr/include/errno.h /usr/include/unistd.h /usr/include/getopt.h
server.o: /usr/include/string.h /usr/include/xlocale.h /usr/include/assert.h
server.o: ../../../include/comm.h ../../../include/common.h
server.o: ../../../include/trains.h ../../../include/address.h
server.o: ../../../include/applicationMessage.h
