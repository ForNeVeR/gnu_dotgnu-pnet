
.PHONY: Mono.GetOptions.dll

all-local: Mono.GetOptions.dll

Mono.GetOptions.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f Mono.GetOptions.build all

CLEANFILES = Mono.GetOptions.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Mono.GetOptions.dll
