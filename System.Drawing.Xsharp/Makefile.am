
.PHONY: System.Drawing.Xsharp.dll

all-local: System.Drawing.Xsharp.dll

System.Drawing.Xsharp.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.Xsharp.build all

CLEANFILES = System.Drawing.Xsharp.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = System.Drawing.Xsharp.dll
