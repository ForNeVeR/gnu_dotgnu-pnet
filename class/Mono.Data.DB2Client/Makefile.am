
.PHONY: Mono.Data.DB2Client.dll

all-local: Mono.Data.DB2Client.dll

Mono.Data.DB2Client.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f Mono.Data.DB2Client.build all

CLEANFILES = Mono.Data.DB2Client.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Mono.Data.DB2Client.dll
