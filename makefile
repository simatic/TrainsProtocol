# Subdirectories containing a makefile
SUBDIRS = $(dir $(wildcard */makefile) $(wildcard */*/makefile) $(wildcard */*/*/makefile))

.PHONY: all clean ${SUBDIRS}

usage:
	@echo POSSIBLE TARGETS :
	@echo all 
	@echo tests
	@echo doxygen
	@echo clean
	@echo cleandoxygen

all: WHATTODO=all
all: ${SUBDIRS}

tests: WHATTODO=tests
tests: $(SUBDIRS)

clean: WHATTODO=clean
clean: ${SUBDIRS}
	for i in '*'~ '*'.bak '*'.tmp; do find . -iname $$i -exec rm -f '{}' \+; done
	rm -f *~ *.bak *.tmp

doxygen:
	doxygen doxy.conf

cleandoxygen:
	rm -rf documentation/doxygen

${SUBDIRS}:
	${MAKE} -C $@ ${WHATTODO}


