
.PHONY: ISymWrapper.dll

all-local: ISymWrapper.dll

ISymWrapper.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f ISymWrapper.build all

CLEANFILES = ISymWrapper.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = ISymWrapper.dll
