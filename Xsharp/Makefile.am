
.PHONY: Xsharp.dll

all-local: Xsharp.dll

Xsharp.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f Xsharp.build all

CLEANFILES = Xsharp.dll
DISTCLEANFILES = Xlib.cs

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Xsharp.dll
