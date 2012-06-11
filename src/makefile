### Configuration.  ####################################################

# Binary to build
BIN		= libtrains.so
# C source files for the library
SOURCES		= $(wildcard *.c)
# Object files corresponding to source files
OBJECTS		= $(SOURCES:.c=.o)
# Include directory
INCLUDEDIRS = -I../include
# Compilation flags (the -fPIC flag is because the files are compiled for a dynamic library)
CFLAGS = -g -Wall $(INCLUDEDIRS) -fPIC -lpthread
# Link flags
LDFLAGS = -g -shared

### Rules.  ############################################################

.PHONY:         all clean cleanall

all: depend $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ 

clean:
	rm -f *~ *.bak *.BAK 

cleanall: clean
	rm -f $(OBJECTS) $(BIN) depend
	# We cut lines after "# DO NOT DELETE THIS LINE" so that
	# we do not keep in GIT dependencies related to a given distribution
	# (e.g. Ubuntu 10.04) which are not compatible with other distributions
	# (e.g. Ubuntu 11.10)
	sed '/^# DO NOT DELETE THIS LINE -- make depend depends on it./q' makefile >makefile.tmp
	mv makefile.tmp makefile

depend: $(SOURCES)
	touch depend
	makedepend -- $(CFLAGS) -- $(SOURCES) 2>/dev/null

# DO NOT DELETE THIS LINE -- make depend depends on it.
