
.PHONY: PEAPI.dll

all-local: PEAPI.dll

PEAPI.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f PEAPI.build all

CLEANFILES = PEAPI.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = PEAPI.dll
