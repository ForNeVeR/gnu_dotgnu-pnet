
noinst_DATA = TestSystem.dll

CSSRC = TestBoolean.cs \
		TestUri.cs \
		TestString.cs \
		TestSystem.cs

CSCC_WARNINGS = -Wno-silly-warnings

CSCC_FLAGS = $(CSCC_WARNINGS) -funsafe -L$(top_srcdir)/csunit -lcstest

TestSystem.dll: $(CSSRC)
	$(CSCC_COMPILE) -o $@ $(CSSRC)

CLEANFILES = TestSystem.dll

TESTS = $(top_srcdir)/tools/run_test.sh
TESTS_ENVIRONMENT = $(SHELL)
