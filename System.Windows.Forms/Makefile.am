
.PHONY: System.Windows.Forms.dll

all-local: System.Windows.Forms.dll

System.Windows.Forms.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f Forms.build all

CLEANFILES = System.Windows.Forms.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.Windows.Forms.dll
