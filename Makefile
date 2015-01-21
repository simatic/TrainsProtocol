# Subdirectories containing a Makefile
SUBDIRS = $(dir $(wildcard */Makefile) $(wildcard */*/Makefile) $(wildcard */*/*/Makefile))

.PHONY: all clean ${SUBDIRS}

usage:
	@echo Possible make targets :
	@echo "\t - all           : compile the library"
	@echo "\t - tests         : compile the library with the tools required for tests"
	@echo "\t - clean         : remove all temporary files"
	@echo "\t - doxygen       : generate the doxygen doc in doc/doxygen"
	@echo "\t - cleandoxygen  : remove the doc/doxygen directory"

all: WHATTODO=all
all: ${SUBDIRS}

tests: WHATTODO=tests
tests: $(SUBDIRS)

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


