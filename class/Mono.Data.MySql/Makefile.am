
.PHONY: Mono.Data.MySql.dll

all-local: Mono.Data.MySql.dll

Mono.Data.MySql.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f Mono.Data.MySql.build all

CLEANFILES = Mono.Data.MySql.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Mono.Data.MySql.dll
