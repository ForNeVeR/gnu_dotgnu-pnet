
.PHONY: System.Runtime.Serialization.Formatters.Soap.dll

all-local: System.Runtime.Serialization.Formatters.Soap.dll

System.Runtime.Serialization.Formatters.Soap.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f System.Runtime.Serialization.Formatters.Soap.build all

CLEANFILES = System.Runtime.Serialization.Formatters.Soap.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.Runtime.Serialization.Formatters.Soap.dll
