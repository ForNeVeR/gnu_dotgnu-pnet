
.PHONY: Custommarshalers.dll

all-local: Custommarshalers.dll

Custommarshalers.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f Custommarshalers.build all

CLEANFILES = Custommarshalers.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Custommarshalers.dll
