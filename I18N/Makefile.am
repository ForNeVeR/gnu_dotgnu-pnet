
REGION_ASSEMBLIES = I18N.CJK.dll I18N.MidEast.dll \
					I18N.Other.dll I18N.Rare.dll \
					I18N.West.dll
HANDLER_FILE = I18N-handlers.def

.PHONY: build-assemblies

all-local: build-assemblies $(HANDLER_FILE)

build-assemblies:
	"$(CSANT)" $(CSANT_FLAGS) -f I18N.build all

I18N.dll: build-assemblies
$(REGION_ASSEMBLIES): build-assemblies

$(HANDLER_FILE): $(REGION_ASSEMBLIES)
	$(SHELL) mklist.sh "$(ILFIND)" $(HANDLER_FILE) $(REGION_ASSEMBLIES)

CLEANFILES = I18N.dll $(REGION_ASSEMBLIES) $(HANDLER_FILE)

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = I18N.dll \
					  $(REGION_ASSEMBLIES)\
					  $(HANDLER_FILE)
