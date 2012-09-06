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
CFLAGS = -Wall -g $(INCLUDEDIRS) $(GLOBALFLAGS)
# Link flags
LDFLAGS = -L../../../src -ltrains -pthread
# Valgrind options
VALGRINDFLAGS =--leak-check=yes --leak-resolution=high --num-callers=40 --tool=memcheck --show-reachable=yes

### Rules.  ############################################################

.PHONY:         all clean

all: depend $(BIN)

tests: GLOBALFLAGS = -DLATENCY_TEST -DINSERTION_TEST
tests: depend $(BIN)

client: client.o
	$(CC) $^ -o $@ $(LDFLAGS)

server: server.o
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *~ *.bak *.BAK *.tmp
	rm -f $(OBJECTS) $(BIN) depend

depend: $(SOURCES)
	touch depend
	makedepend -fdepend -- $(CFLAGS) -- $(SOURCES) 2>/dev/null

# Run all the tests
run: $(BIN)
	(export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../../src; ./basicTest y 1 100000 1)

# Test memory problems with valgrind
valgrind: $(BIN)
	(export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../../src; valgrind $(VALGRINDFLAGS) ./basicTest y 1 100000 1 2>&1)

-include depend
