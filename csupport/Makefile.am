
.PHONY: OpenSystem.C.dll

all-local: OpenSystem.C.dll

OpenSystem.C.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f csupport.build all

CLEANFILES = OpenSystem.C.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = OpenSystem.C.dll
