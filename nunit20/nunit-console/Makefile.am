
.PHONY: nunit-console-pnet.exe

all-local: nunit-console-pnet.exe

nunit-console-pnet.exe:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-Dpnet.libdir="$(PNETLIB_PATH)" \
			-b "$(srcdir)" -B . -f nunit.console.build all

CLEANFILES = nunit-console-pnet.exe

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -b "$(srcdir)" -B . \
			-Dmono.sources="$(MONO_SOURCE_PATH)" \
			-Dpnet.libdir="$(PNETLIB_PATH)" \
			-f "$(srcdir)/nunit.console.build" -i all
	$(LN_S) clrwrap $(DESTDIR)$(bindir)/nunit-console-pnet

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -b "$(srcdir)" -B . \
			-Dmono.sources="$(MONO_SOURCE_PATH)" \
			-Dpnet.libdir="$(PNETLIB_PATH)" \
			-f "$(srcdir)/nunit.console.build" -u all
	rm -f $(DESTDIR)$(bindir)/nunit-console-pnet
