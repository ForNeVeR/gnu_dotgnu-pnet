
.PHONY: Npgsql.dll

all-local: Npgsql.dll

Npgsql.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f Npgsql.build all

CLEANFILES = Npgsql.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Npgsql.dll
