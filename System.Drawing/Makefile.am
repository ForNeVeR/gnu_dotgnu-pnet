
.PHONY: System.Drawing.dll

all-local: System.Drawing.dll

System.Drawing.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.build all

CLEANFILES = System.Drawing.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.Drawing.dll
