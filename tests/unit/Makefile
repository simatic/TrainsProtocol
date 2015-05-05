### Configuration.  ####################################################

UNAME := $(shell uname)

# Binaries to build
BIN		= testAddress testApplicationMessage_wagon testBqueue testComm testList testManagement_addr
# C source files for the binaries
SOURCES		= $(wildcard *.c)
# Include directory
INCLUDEDIRS = -I../../include
# Compilation flags
CFLAGS = -g -Wall $(INCLUDEDIRS) $(GLOBALFLAGS)
# Libraries
ifeq ($(UNAME), Linux)
   # "-Wl,-Bstatic" option is here to force linking with trains static lib ; "-Wl,-Bdynamic" specifies that other libraries will be dynamically linked
   # "-pthread" is required for linking on Linux
   LIBS  = -Wl,-Bstatic -L../../lib -ltrains -Wl,-Bdynamic -pthread
endif
ifeq ($(UNAME), Darwin)
   # On MacOS, one must specify directly the name of the static library we want to link with. Moreover, you must have executed ranlib on this library (http://docstore.mik.ua/orelly/unix3/mac/ch05_05.htm), with "-c" option (that is "ranlib -c", see http://lists.gnu.org/archive/html/libtool/2002-07/msg00025.html)
   # No need for "-pthread" option on MacOS (if you put it, you get a warning)
   LIBS  = ../../lib/libtrains.a
endif
# Link flags
LDFLAGS = -g $(LIBS) 
# Valgrind options
VALGRINDFLAGS =--leak-check=yes --leak-resolution=high --num-callers=40 --tool=memcheck --show-reachable=yes

### Rules.  ############################################################

.PHONY:         all allWithInstrumentation clean run valgrind

all: depend $(BIN)

allWithInstrumentation: GLOBALFLAGS = -DINSERTION_TEST
allWithInstrumentation: depend $(BIN)

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
	rm -f *.o $(BIN) depend

depend: $(SOURCES)
	gcc -M $(CFLAGS) $(SOURCES) >depend 2>/dev/null

# Run all the tests
run: $(BIN)
	for i in $(BIN); do (echo Running $$i; export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../lib; ./$$i); done

# Test memory problems with valgrind
valgrind: $(BIN)
	for i in $(BIN); do (echo Running $$i; export LD_LIBRARY_PATH=LD_LIBRARY_PATH:../../lib; valgrind $(VALGRINDFLAGS) ./$$i 2>&1); done

-include depend
