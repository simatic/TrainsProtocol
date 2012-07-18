### Configuration.  ####################################################

# Binaries to build
BIN		= testAddress testApplicationMessage_wagon testBqueue testComm testList testManagement_addr
# C source files for the binaries
SOURCES		= $(wildcard *.c)
# Include directory
INCLUDEDIRS = -I../../include
# Compilation flags
CFLAGS = -g -Wall $(INCLUDEDIRS)
# Link flags
LDFLAGS = -g -L../../src -ltrains -pthread
# Valgrind options
VALGRINDFLAGS =--leak-check=yes --leak-resolution=high --num-callers=40 --tool=memcheck --show-reachable=yes

### Rules.  ############################################################

.PHONY:         all clean cleanall run valgrind

all: depend $(BIN)

testAddress: testAddress.o
	$(CC) $^ -o $@ $(LDFLAGS)

testApplicationMessage_wagon: testApplicationMessage_wagon.o
	$(CC) $^ -o $@ $(LDFLAGS)

testBqueue: testBqueue.o
	$(CC) $^ -o $@ $(LDFLAGS)

testComm: testComm.o
	$(CC) $^ -o $@ $(LDFLAGS)

testList: testList.o
	$(CC) $^ -o $@ $(LDFLAGS)

testManagement_addr: testManagement_addr.o
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *~ *.bak *.BAK 

cleanall: clean
	rm -f *.o $(BIN) depend

depend: $(SOURCES)
	touch depend
	makedepend -fdepend -- $(CFLAGS) -- $(SOURCES) 2>/dev/null

# Run all the tests
run: $(BIN)
	for i in $(BIN); do (echo Running $$i; export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../src; ./$$i); done

# Test memory problems with valgrind
valgrind: $(BIN)
	for i in $(BIN); do (echo Running $$i; export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../src; valgrind $(VALGRINDFLAGS) ./$$i 2>&1); done

include depend
