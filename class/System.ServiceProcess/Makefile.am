
.PHONY: System.ServiceProcess.dll

all-local: System.ServiceProcess.dll

System.ServiceProcess.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f System.ServiceProcess.build all

CLEANFILES = System.ServiceProcess.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.ServiceProcess.dll
