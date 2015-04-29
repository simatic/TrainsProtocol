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
LDFLAGS = -L../../../lib -ltrains -pthread
# Valgrind options
VALGRINDFLAGS =--leak-check=yes --leak-resolution=high --num-callers=40 --tool=memcheck --show-reachable=yes

### Rules.  ############################################################

.PHONY:         all allWithInstrumentation clean

all: depend $(BIN)

allWithInstrumentation: GLOBALFLAGS = -DINSERTION_TEST
allWithInstrumentation: depend $(BIN)

client: client.o trainTime.o
	$(CC) $^ -o $@ $(LDFLAGS)

server: server.o trainTime.o
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *~ *.bak *.BAK *.tmp
	rm -f $(OBJECTS) $(BIN) depend

depend: $(SOURCES)
	gcc -M $(CFLAGS) $(SOURCES) >depend 2>/dev/null

-include depend
