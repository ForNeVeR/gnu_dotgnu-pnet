
.PHONY: ICSharpCode.SharpZipLib.dll

all-local: ICSharpCode.SharpZipLib.dll

ICSharpCode.SharpZipLib.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f SharpZipLib.build all

CLEANFILES = ICSharpCode.SharpZipLib.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = ICSharpCode.SharpZipLib.dll
