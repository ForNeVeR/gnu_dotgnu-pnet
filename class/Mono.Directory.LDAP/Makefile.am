
.PHONY: Mono.Directory.LDAP.dll

all-local: Mono.Directory.LDAP.dll

Mono.Directory.LDAP.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f Mono.Directory.LDAP.build all

CLEANFILES = Mono.Directory.LDAP.dll

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Mono.Directory.LDAP.dll
