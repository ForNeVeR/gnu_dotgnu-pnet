
.PHONY: ByteFX.Data.dll

all-local: ByteFX.Data.dll

ByteFX.Data.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f ByteFX.Data.build all

CLEANFILES = ByteFX.Data.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = ByteFX.Data.dll
