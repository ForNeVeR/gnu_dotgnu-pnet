
.PHONY: System.Messaging.dll

all-local: System.Messaging.dll

System.Messaging.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f System.Messaging.build all

CLEANFILES = System.Messaging.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.Messaging.dll
