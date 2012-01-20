# Executable to build
BIN = basicTest

# Paths where sources can be found (separate by space, e.g. "dir1 dir2")
SOURCEDIRS = .

# All the sources
SOURCES := $(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.c))

# All the objects
OBJETS= $(subst .c,.o, $(SOURCES))

# Compilation options
INCLUDEDIRSOPTION = -I../../../include $(foreach dir, $(SOURCEDIRS),-I$(dir) )
CFLAGS = -Wall -g -pthread $(INCLUDEDIRSOPTION)

# Valgrind options
VALGRINDFLAGS =--leak-check=yes --leak-resolution=high --num-callers=40 --tool=memcheck --show-reachable=yes


all:  depend $(BIN)

# makedepend : used to write in this Makefile dependencies to all include files
depend: $(SOURCES) 
	touch depend
	makedepend -- $(INCLUDEDIRSOPTION) -- $(SOURCES) 2>/dev/null

# 
$(BIN): $(OBJETS)
	gcc $(CFLAGS) $(OBJETS) -o $(BIN)

# Produces doxygen documentation
doc:
	doxygen doxy.conf

# Test memory leaks with valgrind
valgrind: $(BIN)
	valgrind -v $(VALGRINDFLAGS) ./$(BIN) 2>&1

# Nettoyage du repertoire
clean:
	rm -f *.o */*.o *~ *.bak 

cleanall: clean cleandoc
	rm -f  $(BIN) depend

cleandoc:
	rm -rf html latex
# DO NOT DELETE

./basicTest.o: /usr/include/stdlib.h /usr/include/features.h
./basicTest.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
./basicTest.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
./basicTest.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
./basicTest.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
./basicTest.o: /usr/include/time.h /usr/include/endian.h
./basicTest.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
./basicTest.o: /usr/include/sys/select.h /usr/include/bits/select.h
./basicTest.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
./basicTest.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
./basicTest.o: /usr/include/alloca.h /usr/include/unistd.h
./basicTest.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
./basicTest.o: /usr/include/getopt.h /usr/include/stdio.h
./basicTest.o: /usr/include/libio.h /usr/include/_G_config.h
./basicTest.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
./basicTest.o: /usr/include/bits/sys_errlist.h /usr/include/error.h
./basicTest.o: /usr/include/bits/error.h /usr/include/errno.h
./basicTest.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
./basicTest.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
./basicTest.o: /usr/include/asm-generic/errno-base.h /usr/include/semaphore.h
./basicTest.o: /usr/include/bits/semaphore.h /usr/include/strings.h
./basicTest.o: /usr/include/xlocale.h ../../../include/trains.h
./mock.o: /usr/include/stdio.h /usr/include/features.h
./mock.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
./mock.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
./mock.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
./mock.o: /usr/include/bits/typesizes.h /usr/include/libio.h
./mock.o: /usr/include/_G_config.h /usr/include/wchar.h
./mock.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
./mock.o: ../../../include/trains.h
