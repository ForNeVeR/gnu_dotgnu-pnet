
.PHONY: nunit.util.dll

all-local: nunit.util.dll

nunit.util.dll:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-b "$(srcdir)" -B . -f nunit.util.build all

CLEANFILES = nunit.util.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -b "$(srcdir)" -B . \
			-Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f "$(srcdir)/nunit.util.build" -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -b "$(srcdir)" -B . \
			-Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f "$(srcdir)/nunit.util.build" -u all
