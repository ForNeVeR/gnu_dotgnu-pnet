
.PHONY: Mono.Posix.dll

all-local: Mono.Posix.dll

Mono.Posix.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f Mono.Posix.build all

CLEANFILES = Mono.Posix.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Mono.Posix.dll
