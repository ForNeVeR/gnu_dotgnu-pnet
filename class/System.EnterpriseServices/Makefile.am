
.PHONY: System.EnterpriseServices.dll

all-local: System.EnterpriseServices.dll

System.EnterpriseServices.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f System.EnterpriseServices.build all

CLEANFILES = System.EnterpriseServices.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.EnterpriseServices.dll
