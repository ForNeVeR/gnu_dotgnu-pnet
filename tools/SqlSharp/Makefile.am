
.PHONY: sqlsharp-pnet.exe

all-local: sqlsharp-pnet.exe

sqlsharp-pnet.exe:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-f SqlSharp.build all

CLEANFILES = sqlsharp-pnet.exe

install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) sqlsharp-pnet.exe $(DESTDIR)$(bindir)/sqlsharp-pnet.exe
	$(INSTALL_PROGRAM) $(srcdir)/sqlsharp-run.sh $(DESTDIR)$(bindir)/sqlsharp-pnet
	
uninstall-local:
	@$(NORMAL_UNINSTALL)
	rm -f $(DESTDIR)$(bindir)/sqlsharp-pnet.exe
	rm -f $(DESTDIR)$(bindir)/sqlsharp-pnet
