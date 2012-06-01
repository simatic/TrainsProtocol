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
CFLAGS = -g -Wall $(INCLUDEDIRS) -fPIC 
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

str_train.o: ../include/str_train.h /usr/include/pthread.h
str_train.o: /usr/include/features.h /usr/include/sys/cdefs.h
str_train.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
str_train.o: /usr/include/gnu/stubs-32.h /usr/include/endian.h
str_train.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
str_train.o: /usr/include/sched.h /usr/include/bits/types.h
str_train.o: /usr/include/bits/typesizes.h /usr/include/time.h
str_train.o: /usr/include/bits/sched.h /usr/include/bits/pthreadtypes.h
str_train.o: /usr/include/bits/setjmp.h ../include/address.h
str_train.o: ../include/common.h /usr/include/stdlib.h
str_train.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
str_train.o: /usr/include/sys/types.h /usr/include/sys/select.h
str_train.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
str_train.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
str_train.o: /usr/include/alloca.h /usr/include/error.h
str_train.o: /usr/include/bits/error.h ../include/wagon.h
str_train.o: ../include/applicationMessage.h ../include/bqueue.h
str_train.o: /usr/include/semaphore.h /usr/include/bits/semaphore.h
str_train.o: ../include/list.h
applicationMessage.o: /usr/include/string.h /usr/include/features.h
applicationMessage.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
applicationMessage.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
applicationMessage.o: /usr/include/xlocale.h /usr/include/stdio.h
applicationMessage.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
applicationMessage.o: /usr/include/libio.h /usr/include/_G_config.h
applicationMessage.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
applicationMessage.o: /usr/include/bits/sys_errlist.h
applicationMessage.o: ../include/applicationMessage.h ../include/address.h
applicationMessage.o: ../include/common.h /usr/include/stdlib.h
applicationMessage.o: /usr/include/bits/waitflags.h
applicationMessage.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
applicationMessage.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
applicationMessage.o: /usr/include/sys/types.h /usr/include/time.h
applicationMessage.o: /usr/include/sys/select.h /usr/include/bits/select.h
applicationMessage.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
applicationMessage.o: /usr/include/sys/sysmacros.h
applicationMessage.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
applicationMessage.o: /usr/include/pthread.h /usr/include/sched.h
applicationMessage.o: /usr/include/bits/sched.h /usr/include/bits/setjmp.h
applicationMessage.o: /usr/include/error.h /usr/include/bits/error.h
applicationMessage.o: ../include/trains.h ../include/wagon.h
applicationMessage.o: ../include/bqueue.h /usr/include/semaphore.h
applicationMessage.o: /usr/include/bits/semaphore.h ../include/list.h
applicationMessage.o: ../include/counter.h
comm.o: /usr/include/error.h /usr/include/features.h /usr/include/sys/cdefs.h
comm.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
comm.o: /usr/include/gnu/stubs-32.h /usr/include/bits/error.h
comm.o: /usr/include/assert.h /usr/include/stdlib.h
comm.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
comm.o: /usr/include/endian.h /usr/include/bits/endian.h
comm.o: /usr/include/bits/byteswap.h /usr/include/sys/types.h
comm.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
comm.o: /usr/include/time.h /usr/include/sys/select.h
comm.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
comm.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
comm.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
comm.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
comm.o: /usr/include/bits/environments.h /usr/include/bits/confname.h
comm.o: /usr/include/getopt.h /usr/include/string.h /usr/include/xlocale.h
comm.o: /usr/include/errno.h /usr/include/bits/errno.h
comm.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
comm.o: /usr/include/asm-generic/errno.h
comm.o: /usr/include/asm-generic/errno-base.h /usr/include/sys/socket.h
comm.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
comm.o: /usr/include/bits/socket.h /usr/include/bits/sockaddr.h
comm.o: /usr/include/asm/socket.h /usr/include/asm-generic/socket.h
comm.o: /usr/include/asm/sockios.h /usr/include/asm-generic/sockios.h
comm.o: /usr/include/netdb.h /usr/include/netinet/in.h /usr/include/stdint.h
comm.o: /usr/include/bits/wchar.h /usr/include/bits/in.h
comm.o: /usr/include/rpc/netdb.h /usr/include/bits/netdb.h
comm.o: /usr/include/netinet/tcp.h /usr/include/sys/time.h
comm.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
comm.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
comm.o: /usr/include/bits/sys_errlist.h ../include/comm.h
comm.o: /usr/include/pthread.h /usr/include/sched.h /usr/include/bits/sched.h
comm.o: /usr/include/bits/setjmp.h ../include/common.h ../include/signalMgt.h
comm.o: /usr/include/signal.h /usr/include/bits/signum.h
comm.o: /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h
comm.o: /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h
comm.o: /usr/include/linux/types.h /usr/include/asm/types.h
comm.o: /usr/include/asm-generic/types.h /usr/include/asm-generic/int-ll64.h
comm.o: /usr/include/asm/bitsperlong.h /usr/include/asm-generic/bitsperlong.h
comm.o: /usr/include/linux/posix_types.h /usr/include/linux/stddef.h
comm.o: /usr/include/asm/posix_types.h /usr/include/asm/posix_types_32.h
comm.o: /usr/include/bits/sigstack.h /usr/include/sys/ucontext.h
comm.o: /usr/include/bits/sigthread.h
list.o: /usr/include/error.h /usr/include/features.h /usr/include/sys/cdefs.h
list.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
list.o: /usr/include/gnu/stubs-32.h /usr/include/bits/error.h
list.o: /usr/include/assert.h /usr/include/stdlib.h
list.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
list.o: /usr/include/endian.h /usr/include/bits/endian.h
list.o: /usr/include/bits/byteswap.h /usr/include/sys/types.h
list.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
list.o: /usr/include/time.h /usr/include/sys/select.h
list.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
list.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
list.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
list.o: ../include/list.h /usr/include/pthread.h /usr/include/sched.h
list.o: /usr/include/bits/sched.h /usr/include/bits/setjmp.h
list.o: ../include/common.h
wagon.o: /usr/include/stdlib.h /usr/include/features.h
wagon.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
wagon.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
wagon.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
wagon.o: /usr/include/endian.h /usr/include/bits/endian.h
wagon.o: /usr/include/bits/byteswap.h /usr/include/sys/types.h
wagon.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
wagon.o: /usr/include/time.h /usr/include/sys/select.h
wagon.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
wagon.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
wagon.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
wagon.o: /usr/include/pthread.h /usr/include/sched.h
wagon.o: /usr/include/bits/sched.h /usr/include/bits/setjmp.h
wagon.o: /usr/include/assert.h ../include/bqueue.h /usr/include/semaphore.h
wagon.o: /usr/include/bits/semaphore.h ../include/list.h ../include/wagon.h
wagon.o: ../include/address.h ../include/common.h /usr/include/error.h
wagon.o: /usr/include/bits/error.h ../include/applicationMessage.h
msg.o: /usr/include/stdlib.h /usr/include/features.h /usr/include/sys/cdefs.h
msg.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
msg.o: /usr/include/gnu/stubs-32.h /usr/include/bits/waitflags.h
msg.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
msg.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
msg.o: /usr/include/sys/types.h /usr/include/bits/types.h
msg.o: /usr/include/bits/typesizes.h /usr/include/time.h
msg.o: /usr/include/sys/select.h /usr/include/bits/select.h
msg.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
msg.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
msg.o: /usr/include/alloca.h /usr/include/stdio.h /usr/include/libio.h
msg.o: /usr/include/_G_config.h /usr/include/wchar.h
msg.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
msg.o: ../include/address.h ../include/common.h /usr/include/pthread.h
msg.o: /usr/include/sched.h /usr/include/bits/sched.h
msg.o: /usr/include/bits/setjmp.h /usr/include/error.h
msg.o: /usr/include/bits/error.h ../include/msg.h ../include/str_train.h
msg.o: ../include/wagon.h ../include/applicationMessage.h ../include/bqueue.h
msg.o: /usr/include/semaphore.h /usr/include/bits/semaphore.h
msg.o: ../include/list.h
address.o: /usr/include/stdio.h /usr/include/features.h
address.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
address.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
address.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
address.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
address.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
address.o: ../include/address.h ../include/common.h /usr/include/stdlib.h
address.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
address.o: /usr/include/endian.h /usr/include/bits/endian.h
address.o: /usr/include/bits/byteswap.h /usr/include/sys/types.h
address.o: /usr/include/time.h /usr/include/sys/select.h
address.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
address.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
address.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
address.o: /usr/include/pthread.h /usr/include/sched.h
address.o: /usr/include/bits/sched.h /usr/include/bits/setjmp.h
address.o: /usr/include/error.h /usr/include/bits/error.h
counter.o: ../include/counter.h
signalMgt.o: /usr/include/stdio.h /usr/include/features.h
signalMgt.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
signalMgt.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
signalMgt.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
signalMgt.o: /usr/include/libio.h /usr/include/_G_config.h
signalMgt.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
signalMgt.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
signalMgt.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
signalMgt.o: /usr/include/endian.h /usr/include/bits/endian.h
signalMgt.o: /usr/include/bits/byteswap.h /usr/include/sys/types.h
signalMgt.o: /usr/include/time.h /usr/include/sys/select.h
signalMgt.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
signalMgt.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
signalMgt.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
signalMgt.o: /usr/include/error.h /usr/include/bits/error.h
signalMgt.o: /usr/include/errno.h /usr/include/bits/errno.h
signalMgt.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
signalMgt.o: /usr/include/asm-generic/errno.h
signalMgt.o: /usr/include/asm-generic/errno-base.h /usr/include/pthread.h
signalMgt.o: /usr/include/sched.h /usr/include/bits/sched.h
signalMgt.o: /usr/include/bits/setjmp.h /usr/include/unistd.h
signalMgt.o: /usr/include/bits/posix_opt.h /usr/include/bits/environments.h
signalMgt.o: /usr/include/bits/confname.h /usr/include/getopt.h
signalMgt.o: /usr/include/sys/syscall.h /usr/include/asm/unistd.h
signalMgt.o: /usr/include/asm/unistd_32.h /usr/include/bits/syscall.h
signalMgt.o: ../include/signalMgt.h /usr/include/signal.h
signalMgt.o: /usr/include/bits/signum.h /usr/include/bits/siginfo.h
signalMgt.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
signalMgt.o: /usr/include/asm/sigcontext.h /usr/include/linux/types.h
signalMgt.o: /usr/include/asm/types.h /usr/include/asm-generic/types.h
signalMgt.o: /usr/include/asm-generic/int-ll64.h
signalMgt.o: /usr/include/asm/bitsperlong.h
signalMgt.o: /usr/include/asm-generic/bitsperlong.h
signalMgt.o: /usr/include/linux/posix_types.h /usr/include/linux/stddef.h
signalMgt.o: /usr/include/asm/posix_types.h /usr/include/asm/posix_types_32.h
signalMgt.o: /usr/include/bits/sigstack.h /usr/include/sys/ucontext.h
signalMgt.o: /usr/include/bits/sigthread.h ../include/common.h
bqueue.o: /usr/include/error.h /usr/include/features.h
bqueue.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
bqueue.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
bqueue.o: /usr/include/bits/error.h /usr/include/errno.h
bqueue.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
bqueue.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
bqueue.o: /usr/include/asm-generic/errno-base.h /usr/include/assert.h
bqueue.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
bqueue.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
bqueue.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
bqueue.o: /usr/include/sys/types.h /usr/include/bits/types.h
bqueue.o: /usr/include/bits/typesizes.h /usr/include/time.h
bqueue.o: /usr/include/sys/select.h /usr/include/bits/select.h
bqueue.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
bqueue.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
bqueue.o: /usr/include/alloca.h ../include/bqueue.h /usr/include/semaphore.h
bqueue.o: /usr/include/bits/semaphore.h ../include/list.h
bqueue.o: /usr/include/pthread.h /usr/include/sched.h
bqueue.o: /usr/include/bits/sched.h /usr/include/bits/setjmp.h
iomsg.o: /usr/include/stdlib.h /usr/include/features.h
iomsg.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
iomsg.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
iomsg.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
iomsg.o: /usr/include/endian.h /usr/include/bits/endian.h
iomsg.o: /usr/include/bits/byteswap.h /usr/include/sys/types.h
iomsg.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
iomsg.o: /usr/include/time.h /usr/include/sys/select.h
iomsg.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
iomsg.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
iomsg.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
iomsg.o: ../include/address.h ../include/common.h /usr/include/pthread.h
iomsg.o: /usr/include/sched.h /usr/include/bits/sched.h
iomsg.o: /usr/include/bits/setjmp.h /usr/include/error.h
iomsg.o: /usr/include/bits/error.h ../include/management_addr.h
iomsg.o: ../include/comm.h /usr/include/sys/uio.h /usr/include/bits/uio.h
iomsg.o: ../include/str_train.h ../include/wagon.h
iomsg.o: ../include/applicationMessage.h ../include/bqueue.h
iomsg.o: /usr/include/semaphore.h /usr/include/bits/semaphore.h
iomsg.o: ../include/list.h ../include/msg.h
management_addr.o: /usr/include/stdlib.h /usr/include/features.h
management_addr.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
management_addr.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
management_addr.o: /usr/include/bits/waitflags.h
management_addr.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
management_addr.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
management_addr.o: /usr/include/sys/types.h /usr/include/bits/types.h
management_addr.o: /usr/include/bits/typesizes.h /usr/include/time.h
management_addr.o: /usr/include/sys/select.h /usr/include/bits/select.h
management_addr.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
management_addr.o: /usr/include/sys/sysmacros.h
management_addr.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
management_addr.o: /usr/include/string.h /usr/include/xlocale.h
management_addr.o: /usr/include/stdio.h /usr/include/libio.h
management_addr.o: /usr/include/_G_config.h /usr/include/wchar.h
management_addr.o: /usr/include/bits/stdio_lim.h
management_addr.o: /usr/include/bits/sys_errlist.h ../include/comm.h
management_addr.o: /usr/include/pthread.h /usr/include/sched.h
management_addr.o: /usr/include/bits/sched.h /usr/include/bits/setjmp.h
management_addr.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
management_addr.o: ../include/common.h /usr/include/error.h
management_addr.o: /usr/include/bits/error.h
