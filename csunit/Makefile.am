
noinst_DATA = csunit.exe

CSSRC = Test.cs \
		TestArray.cs \
		TestAssertFailed.cs \
		TestCase.cs \
		TestException.cs \
		TestFailure.cs \
		TestMain.cs \
		TestResult.cs \
		TestStop.cs \
		TestSuite.cs \
		TestVersion.cs \
		TestWriterResult.cs

CSCC_WARNINGS = -Wno-silly-warnings

CSCC_FLAGS = $(CSCC_WARNINGS) -funsafe $(CSCC_EXE_LIBS)

csunit.exe: $(CSSRC) $(DEFRESOURCES)
	$(CSCC_COMPILE) -o $@ $(CSSRC)

CLEANFILES = csunit.exe TestVersion.cs
