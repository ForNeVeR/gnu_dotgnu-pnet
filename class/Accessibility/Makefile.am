
.PHONY: Accessibility.dll

all-local: Accessibility.dll

Accessibility.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f Accessibility.build all

CLEANFILES = Accessibility.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Accessibility.dll
