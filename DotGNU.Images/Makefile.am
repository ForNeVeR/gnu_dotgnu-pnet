
.PHONY: DotGNU.Images.dll

all-local: DotGNU.Images.dll

DotGNU.Images.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f DotGNU.Images.build all

CLEANFILES = DotGNU.Images.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = DotGNU.Images.dll
