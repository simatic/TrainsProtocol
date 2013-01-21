### Configuration.  ####################################################

GCCMAKEDEP ?= gccmakedep

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
INCLUDEDIRS = -I../../../include -I$(JAVA_HOME)/include
# Compilation flags (the -fPIC flag is because the files are compiled for a dynamic library)
CFLAGS = -g -Wall $(INCLUDEDIRS) $(GLOBALFLAGS) -fPIC -lpthread 
# Link flags
LDFLAGS = -g -shared

### Rules.  ############################################################

.PHONY:         all clean

all: depend $(BIN)

tests: GLOBALFLAGS = -DLATENCY_TEST -DINSERTION_TEST
tests: depend $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ 

clean:
	rm -f *~ *.bak *.BAK 
	rm -f $(OBJECTS) $(BIN) depend

depend: $(SOURCES)
	touch depend
	$(GCCMAKEDEP) -fdepend -- $(CFLAGS) -- $(SOURCES)

-include depend
