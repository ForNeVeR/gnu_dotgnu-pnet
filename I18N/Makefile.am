
REGION_ASSEMBLIES = I18N.CJK.dll I18N.MidEast.dll \
					I18N.Other.dll I18N.Rare.dll \
					I18N.West.dll
HANDLER_FILE = I18N-handlers.def

.PHONY: phase-one phase-two

all-local: phase-one $(HANDLER_FILE) phase-two

phase-one:
	test -d first || mkdir first
	"$(CSANT)" $(CSANT_FLAGS) -f I18N.build all

phase-two:
	"$(CSANT)" $(CSANT_FLAGS) -f I18N.build phase-two

$(HANDLER_FILE): first/I18N.dll $(REGION_ASSEMBLIES)
	$(SHELL) mklist.sh "$(ILFIND)" $(HANDLER_FILE) \
			first/I18N.dll $(REGION_ASSEMBLIES)

CLEANFILES = $(REGION_ASSEMBLIES) $(HANDLER_FILE) I18N.dll first/I18N.dll

install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	$(INSTALL_DATA) I18N.dll $(DESTDIR)$(libdir)/cscc/lib/I18N.dll
	$(INSTALL_DATA) I18N.CJK.dll $(DESTDIR)$(libdir)/cscc/lib/I18N.CJK.dll
	$(INSTALL_DATA) I18N.MidEast.dll $(DESTDIR)$(libdir)/cscc/lib/I18N.MidEast.dll
	$(INSTALL_DATA) I18N.Other.dll $(DESTDIR)$(libdir)/cscc/lib/I18N.Other.dll
	$(INSTALL_DATA) I18N.Rare.dll $(DESTDIR)$(libdir)/cscc/lib/I18N.Rare.dll
	$(INSTALL_DATA) I18N.West.dll $(DESTDIR)$(libdir)/cscc/lib/I18N.West.dll
	rm -f $(DESTDIR)$(libdir)/cscc/lib/I18N-handlers.def

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(libdir)/cscc/lib/I18N.dll
	rm -f $(DESTDIR)$(libdir)/cscc/lib/I18N.CJK.dll
	rm -f $(DESTDIR)$(libdir)/cscc/lib/I18N.MidEast.dll
	rm -f $(DESTDIR)$(libdir)/cscc/lib/I18N.Other.dll
	rm -f $(DESTDIR)$(libdir)/cscc/lib/I18N.Rare.dll
	rm -f $(DESTDIR)$(libdir)/cscc/lib/I18N.West.dll
