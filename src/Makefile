### Configuration.  ####################################################


UNAME := $(shell uname)

# Binary to build
ifeq ($(UNAME), Linux)
	BIN  = ../lib/libtrains.so
endif
ifeq ($(UNAME), Darwin)
	BIN  = ../lib/libtrains.jnilib
endif
# C source files for the library
SOURCES  = $(wildcard *.c)
# Object files corresponding to source files
OBJECTS  = $(SOURCES:.c=.o)
# Include directory
INCLUDEDIRS = -I../include -I$(JAVA_HOME)/include
# Compilation flags (the -fPIC flag is because the files are compiled for a dynamic library)
CFLAGS = -g -Wall $(INCLUDEDIRS) $(GLOBALFLAGS) -fPIC -pthread 
# Link flags
LDFLAGS = -g -shared

### Rules.  ############################################################

.PHONY:         all allForJNI clean

all: depend $(BIN)

allForJNI: GLOBALFLAGS = -DJNI
allForJNI: depend $(BIN)

tests: GLOBALFLAGS = -DLATENCY_TEST -DINSERTION_TEST
tests: depend $(BIN)

$(BIN): $(OBJECTS)
	mkdir -p ../lib
	$(CC) $(LDFLAGS) $^ -o $@ 

clean:
	rm -f *~ *.bak *.BAK 
	rm -f $(OBJECTS) $(BIN) depend

depend: $(SOURCES)
	gcc -M $(CFLAGS) $(SOURCES) >depend 2>/dev/null

-include depend
