
noinst_DATA  = cstest.dll csunit.exe

cstest_CSSRC = $(srcdir)/Test.cs \
		       $(srcdir)/TestArray.cs \
		       $(srcdir)/TestAssertFailed.cs \
		       $(srcdir)/TestCase.cs \
		       $(srcdir)/TestException.cs \
		       $(srcdir)/TestFailure.cs \
		       $(srcdir)/TestResult.cs \
		       $(srcdir)/TestStop.cs \
		       $(srcdir)/TestSuite.cs \
		       $(srcdir)/TestWriterResult.cs \
		       $(srcdir)/NUnitEmulation.cs

csunit_CSSRC = $(srcdir)/TestMain.cs \
			   TestVersion.cs

CSCC         = ../cscc/cscc
CSFLAGS      = -g -O2 -funsafe -filasm-path=../ilasm/ilasm \
		       -fplugin-cs-path=../cscc/cscc-cs -L$(top_srcdir)/samples

cstest.dll: $(cstest_CSSRC)
	$(CSCC) $(CSFLAGS) -o $@ $(cstest_CSSRC)

csunit.exe: $(csunit_CSSRC) cstest.dll
	$(CSCC) $(CSFLAGS) -L. -lcstest -o $@ $(csunit_CSSRC)

CLEANFILES = cstest.dll csunit.exe
DISTCLEANFILES = TestVersion.cs

install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	$(INSTALL_DATA) cstest.dll $(DESTDIR)$(libdir)/cscc/lib/cstest.dll
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) csunit.exe $(DESTDIR)$(bindir)/csunit.exe
	$(INSTALL_PROGRAM) $(srcdir)/csunit-run.sh $(DESTDIR)$(bindir)/csunit

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(libdir)/cscc/lib/cstest.dll
	rm -f $(DESTDIR)$(bindir)/csunit.exe
	rm -f $(DESTDIR)$(bindir)/csunit
