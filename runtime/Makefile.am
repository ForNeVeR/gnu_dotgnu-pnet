
.PHONY: mscorlib.dll

all-local: mscorlib.dll

mscorlib.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f runtime.build all

CLEANFILES = mscorlib.dll mscorlib_ecma.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = mscorlib.dll \
					  pnetlib.here
