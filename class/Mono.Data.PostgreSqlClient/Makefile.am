
.PHONY: Mono.Data.PostgreSqlClient.dll

all-local: Mono.Data.PostgreSqlClient.dll

Mono.Data.PostgreSqlClient.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f Mono.Data.PostgreSqlClient.build all

CLEANFILES = Mono.Data.PostgreSqlClient.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Mono.Data.PostgreSqlClient.dll
