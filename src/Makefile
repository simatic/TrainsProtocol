### Configuration.  ####################################################


UNAME := $(shell uname)

# Binary to build
ifeq ($(UNAME), Linux)
	BIN  = libtrains.so
	OS = LINUX
	PIC = -fPIC
else ifeq ($(UNAME), Darwin)
	BIN  = libtrains.jnilib
	OS = DARWIN
	PIC = -fPIC
else ifneq (,$(findstring MINGW32,$(UNAME)))
	BIN = libtrains.dll
	OS = WINDOWS
	PIC =
else
	$(error OS $(UNAME) is not supported for compilation)
endif
# C source files for the library
SOURCES  = $(wildcard *.c)
# Object files corresponding to source files
OBJECTS  = $(SOURCES:.c=.o)
# Include directory
INCLUDEDIRS = -I../include -I../../../pthreads-w32-2-9-1-release/pthreads.2 -I$(JAVA_HOME)/include
# Compilation flags (the -fPIC flag is because the files are compiled for a dynamic library)
CFLAGS = -g -Wall $(INCLUDEDIRS) $(GLOBALFLAGS) -D$(OS) $(PIC) -lpthread 
# Link flags
LDFLAGS = -g -shared

### Rules.  ############################################################

.PHONY:         all allForJNI clean

#all: depend $(BIN)
all: $(BIN)
	echo $(UNAME)

allForJNI: GLOBALFLAGS = -DJNI
allForJNI: depend $(BIN)

tests: GLOBALFLAGS = -DLATENCY_TEST -DINSERTION_TEST
tests: depend $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ 

clean:
	rm -f *~ *.bak *.BAK 
	rm -f $(OBJECTS) $(BIN) depend

depend: $(SOURCES)
	gcc -M $(CFLAGS) $(SOURCES) >depend 2>/dev/null

#-include depend
