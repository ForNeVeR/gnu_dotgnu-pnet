
noinst_DATA = cstest.dll csunit.exe

cstest_CSSRC = Test.cs \
		       TestArray.cs \
		       TestAssertFailed.cs \
		       TestCase.cs \
		       TestException.cs \
		       TestFailure.cs \
		       TestResult.cs \
		       TestStop.cs \
		       TestSuite.cs \
		       TestWriterResult.cs

csunit_CSSRC = TestMain.cs \
			   TestVersion.cs

CSCC_WARNINGS = -Wno-silly-warnings

CSCC_FLAGS = $(CSCC_WARNINGS) -funsafe

cstest.dll: $(cstest_CSSRC)
	$(CSCC_COMPILE) -o $@ $(cstest_CSSRC)

csunit.exe: $(CSSRC) $(DEFRESOURCES)
	$(CSCC_COMPILE) $(CSCC_EXE_LIBS) -L. -lcstest -o $@ $(csunit_CSSRC)

CLEANFILES = cstest.dll csunit.exe
DISTCLEANFILES = TestVersion.cs
