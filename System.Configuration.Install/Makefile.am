
.PHONY: System.Configuration.Install.dll

all-local: System.Configuration.Install.dll

System.Configuration.Install.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f System.Configuration.Install.build all

CLEANFILES = System.Configuration.Install.dll ilinstall.exe

install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f System.Configuration.Install.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) ilinstall.exe $(DESTDIR)$(bindir)/ilinstall.exe
	$(INSTALL_SCRIPT) $(srcdir)/ilinstall-run.sh $(DESTDIR)$(bindir)/ilinstall

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f System.Configuration.Install.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
	rm -f $(DESTDIR)$(bindir)/ilinstall.exe
	rm -f $(DESTDIR)$(bindir)/ilinstall
