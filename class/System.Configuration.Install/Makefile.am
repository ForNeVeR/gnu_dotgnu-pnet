
.PHONY: System.Configuration.Install.dll

all-local: System.Configuration.Install.dll

System.Configuration.Install.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f System.Configuration.Install.build all

CLEANFILES = System.Configuration.Install.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.Configuration.Install.dll
