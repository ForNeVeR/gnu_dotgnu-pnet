
.PHONY: System.Data.OracleClient.dll

all-local: System.Data.OracleClient.dll

System.Data.OracleClient.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f System.Data.OracleClient.build all

CLEANFILES = System.Data.OracleClient.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.Data.OracleClient.dll
