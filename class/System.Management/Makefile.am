
.PHONY: System.Management.dll

all-local: System.Management.dll

System.Management.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f System.Management.build all

CLEANFILES = System.Management.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.Management.dll
