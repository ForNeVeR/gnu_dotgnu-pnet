
.PHONY: System.Configuration.Install.dll

all-local: System.Configuration.Install.dll

System.Configuration.Install.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f System.Configuration.Install.build all

CLEANFILES = System.Configuration.Install.dll ilinstall.exe

install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	$(INSTALL_DATA) System.Configuration.Install.dll \
			$(DESTDIR)$(libdir)/cscc/lib/System.Configuration.Install.dll
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) ilinstall.exe $(DESTDIR)$(bindir)/ilinstall.exe
	$(INSTALL_PROGRAM) $(srcdir)/ilinstall-run.sh $(DESTDIR)$(bindir)/ilinstall

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(libdir)/cscc/lib/System.Configuration.Install.dll
	rm -f $(DESTDIR)$(bindir)/ilinstall.exe
	rm -f $(DESTDIR)$(bindir)/ilinstall
