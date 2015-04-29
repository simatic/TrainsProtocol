# Subdirectories containing a Makefile
SUBDIRS = $(dir $(wildcard */Makefile) $(wildcard */*/Makefile) $(wildcard */*/*/Makefile))

.PHONY: all allWithInstrumentation clean doxygen cleandoxygen ${SUBDIRS}

usage:
	@echo Possible make targets :
	@echo "   - all                    : compile the middleware library and its tests"
	@echo "   - allWithInstrumentation : compile the middleware library and its tests, with instrumentation code (which slows down the library, but allows fine-grained performance measures)"
	@echo "   - clean                  : remove all temporary files"
	@echo "   - doxygen                : generate the doxygen doc in doc/doxygen"
	@echo "   - cleandoxygen           : remove the doc/doxygen directory"

all: WHATTODO=all
all: ${SUBDIRS}

allWithInstrumentation: WHATTODO=allWithInstrumentation
allWithInstrumentation: $(SUBDIRS)

clean: WHATTODO=clean
clean: ${SUBDIRS}
	for i in '*'~ '*'.bak '*'.tmp; do find . -iname $$i -exec rm -f '{}' \+; done
	rm -f *~ *.bak *.tmp
	rm -rf lib

doxygen:
	doxygen doxy.conf

cleandoxygen:
	rm -rf doc/doxygen

${SUBDIRS}:
	${MAKE} -C $@ ${WHATTODO}


