
## System.dll is built in two phases, because there is a circular
## dependency in the System.Configuration namespace involving System.Xml.
##
## During the first phase, "first/System.dll" is built, with no dependency
## upon "System.Xml.dll".  After the top-level Makefile.am has finished
## building "System.Xml", it re-enters this directory and builds
## "System.dll" for real.
##
## The macro "SECOND_PASS" is used in the code to indicate definitions
## that can only be compiled into "System.dll" on the second pass.

.PHONY: System.dll phase-one phase-two

all-local: phase-one

System.dll: phase-one

phase-one:
	test -d first || mkdir first
	test -f first/pnetlib.here || cp pnetlib.here first/pnetlib.here
	"$(CSANT)" $(CSANT_FLAGS) -f System.build all

phase-two:
	"$(CSANT)" $(CSANT_FLAGS) -f System.build phase-two

CLEANFILES = System.dll first/System.dll first/pnetlib.here

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.dll
