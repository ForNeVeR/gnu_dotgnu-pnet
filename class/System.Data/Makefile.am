
.PHONY: System.Data.dll

all-local: System.Data.dll

System.Data.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f System.Data.build all

CLEANFILES = System.Data.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.Data.dll
