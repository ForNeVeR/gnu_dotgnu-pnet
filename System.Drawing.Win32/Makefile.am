
.PHONY: System.Drawing.Win32.dll

all-local: System.Drawing.Win32.dll

System.Drawing.Win32.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.Win32.build all

CLEANFILES = System.Drawing.Win32.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.Drawing.Win32.dll
