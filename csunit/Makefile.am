
.PHONY: csunit

all-local: csunit

csunit: TestVersion.cs
	"$(CSANT)" $(CSANT_FLAGS) -f csunit.build all

CLEANFILES = cstest.dll csunit.exe
DISTCLEANFILES = TestVersion.cs

install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f csunit.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) csunit.exe $(DESTDIR)$(bindir)/csunit.exe
	$(INSTALL_SCRIPT) $(srcdir)/csunit-run.sh $(DESTDIR)$(bindir)/csunit

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f csunit.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
	rm -f $(DESTDIR)$(bindir)/csunit.exe
	rm -f $(DESTDIR)$(bindir)/csunit
