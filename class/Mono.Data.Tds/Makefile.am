
.PHONY: Mono.Data.Tds.dll

all-local: Mono.Data.Tds.dll

Mono.Data.Tds.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f Mono.Data.Tds.build all

CLEANFILES = Mono.Data.Tds.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Mono.Data.Tds.dll
