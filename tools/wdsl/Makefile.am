
.PHONY: wdsl-pnet.exe

all-local: wdsl-pnet.exe

wdsl-pnet.exe:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-Dpnet.libdir="$(PNETLIB_PATH)" \
			-b "$(srcdir)" -B . -f wdsl.build all

CLEANFILES = wdsl-pnet.exe

install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -b "$(srcdir)" -B . \
			-Dmono.sources="$(MONO_SOURCE_PATH)" \
			-Dpnet.libdir="$(PNETLIB_PATH)" \
			-f "$(srcdir)/wdsl.build" -i all
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	rm -f $(DESTDIR)$(bindir)/wdsl-pnet.exe
	rm -f $(DESTDIR)$(bindir)/wdsl-pnet
	$(LN_S) clrwrap $(DESTDIR)$(bindir)/wdsl-pnet

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -b "$(srcdir)" -B . \
			-Dmono.sources="$(MONO_SOURCE_PATH)" \
			-Dpnet.libdir="$(PNETLIB_PATH)" \
			-f "$(srcdir)/wdsl.build" -u all
	rm -f $(DESTDIR)$(bindir)/wdsl-pnet.exe
	rm -f $(DESTDIR)$(bindir)/wdsl-pnet
