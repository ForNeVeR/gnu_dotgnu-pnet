
.PHONY: nunit-console.exe

all-local: nunit-console.exe

nunit-console.exe:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-b "$(srcdir)" -B . -f nunit.console.build all

CLEANFILES = nunit-console.exe

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -b "$(srcdir)" -B . \
			-Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f "$(srcdir)/nunit.console.build" -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -b "$(srcdir)" -B . \
			-Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f "$(srcdir)/nunit.console.build" -u all
