
.PHONY: DotGNU.SSL.dll

all-local: DotGNU.SSL.dll

DotGNU.SSL.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f SSL.build all

CLEANFILES = DotGNU.SSL.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = DotGNU.SSL.dll
