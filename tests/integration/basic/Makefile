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
# Link flags ("-Wl,-Bstatic" option is here to force linking with trains static lib ; "-Wl,-Bdynamic" specifies that othe rlibraries will be dynamically linked)
LDFLAGS = -g -pthread -Wl,-Bstatic -L../../../lib -ltrains -Wl,-Bdynamic
# Valgrind options
VALGRINDFLAGS =--leak-check=yes --leak-resolution=high --num-callers=40 --tool=memcheck --show-reachable=yes

### Rules.  ############################################################

.PHONY:         all allWithInstrumentation clean run valgrind

all: depend $(BIN)

allWithInstrumentation: GLOBALFLAGS = -DINSERTION_TEST
allWithInstrumentation: depend $(BIN)

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
