# Subdirectories containing a makefile
SUBDIRS = $(dir $(wildcard */makefile) $(wildcard */*/makefile) $(wildcard */*/*/makefile))

.PHONY: all clean ${SUBDIRS}

all: WHATTODO=all
all: ${SUBDIRS}

tests: WHATTODO=tests
tests: $(SUBDIRS)

clean: WHATTODO=clean
clean: ${SUBDIRS}
	for i in '*'~ '*'.bak '*'.tmp; do find . -iname $$i -exec rm -f '{}' \+; done
	rm -f *~ *.bak *.tmp

doc:
	doxygen doxy.conf

cleandoc:
	rm -rf html latex

${SUBDIRS}:
	${MAKE} -C $@ ${WHATTODO}


