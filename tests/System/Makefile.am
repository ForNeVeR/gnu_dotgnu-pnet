
noinst_DATA = TestSystem.dll

CSSRC = TestBoolean.cs \
		TestSystem.cs

CSCC_WARNINGS = -Wno-silly-warnings

CSCC_FLAGS = $(CSCC_WARNINGS) -funsafe -L$(top_srcdir)/csunit -lcstest

TestSystem.dll: $(CSSRC)
	$(CSCC_COMPILE) -o $@ $(CSSRC)

CLEANFILES = TestSystem.dll
