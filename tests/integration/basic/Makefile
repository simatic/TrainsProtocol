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
CFLAGS = -Wall -g $(INCLUDEDIRS) $(GLOBALFLAGS)
# Link flags
LDFLAGS = -L../../../lib -ltrains -pthread
# Valgrind options
VALGRINDFLAGS =--leak-check=yes --leak-resolution=high --num-callers=40 --tool=memcheck --show-reachable=yes

### Rules.  ############################################################

.PHONY:         all clean run valgrind

all: depend $(BIN)

tests: GLOBALFLAGS = -DINSERTION_TEST
tests: depend $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *~ *.bak *.BAK *.tmp
	rm -f $(OBJECTS) $(BIN) depend

depend: $(SOURCES)
	gcc -M $(CFLAGS) $(SOURCES) >depend 2>/dev/null

# Run all the tests
run: $(BIN)
	(export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../../lib; ./$(BIN) y 1 100000 1)

# Test memory problems with valgrind
valgrind: $(BIN)
	(export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../../lib; valgrind $(VALGRINDFLAGS) ./$(BIN) y 1 100000 1 2>&1)

-include depend
