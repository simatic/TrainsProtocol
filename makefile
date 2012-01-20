all:
	@echo For the moment, this makefile produces only doxygen documentation
	@echo When the project will be done, it will produce:
	@echo  - the middleware library
	@echo  - all the tests
	@echo
	doxygen doxy.conf

clean:
	rm -f *~ *.bak */*~ */*.bak

cleandoc:
	rm -rf html latex

cleanall: clean cleandoc

