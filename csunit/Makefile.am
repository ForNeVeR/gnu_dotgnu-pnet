
.PHONY: csunit

all-local: csunit

csunit: TestVersion.cs
	"$(CSANT)" $(CSANT_FLAGS) -f csunit.build all

CLEANFILES = cstest.dll csunit.exe
DISTCLEANFILES = TestVersion.cs

install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	$(INSTALL_DATA) cstest.dll $(DESTDIR)$(libdir)/cscc/lib/cstest.dll
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) csunit.exe $(DESTDIR)$(bindir)/csunit.exe
	$(INSTALL_SCRIPT) $(srcdir)/csunit-run.sh $(DESTDIR)$(bindir)/csunit

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(libdir)/cscc/lib/cstest.dll
	rm -f $(DESTDIR)$(bindir)/csunit.exe
	rm -f $(DESTDIR)$(bindir)/csunit
