
.PHONY: DotGNU.Terminal.dll

all-local: DotGNU.Terminal.dll

DotGNU.Terminal.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f DotGNU.Terminal.build all

CLEANFILES = DotGNU.Terminal.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = DotGNU.Terminal.dll
