### Configuration.  ####################################################

UNAME := $(shell uname)

# Binary to build
ifeq ($(UNAME), Linux)
	BIN  = libperf.so
endif
ifeq ($(UNAME), Darwin)
	BIN  = libperf.jnilib
endif
# C source files for the library
SOURCES  = $(wildcard *.c)
# Object files corresponding to source files
OBJECTS  = $(SOURCES:.c=.o)
# Include directory
INCLUDEDIRS = -I../../../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/darwin
# Compilation flags (the -fPIC flag is because the files are compiled for a dynamic library)
CFLAGS = -g -Wall $(INCLUDEDIRS) $(GLOBALFLAGS) -fPIC -pthread 
# Link flags
LDFLAGS = -g -shared

### Rules.  ############################################################

.PHONY:         all allWithInstrumentation clean

all: depend $(BIN)

allWithInstrumentation: GLOBALFLAGS = -DINSERTION_TEST
allWithInstrumentation: depend $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ 

clean:
	rm -f *~ *.bak *.BAK 
	rm -f $(OBJECTS) $(BIN) depend

depend: $(SOURCES)
	gcc -M $(CFLAGS) $(SOURCES) >depend 2>/dev/null

-include depend
