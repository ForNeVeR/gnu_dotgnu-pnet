
all-local:
	"$(CSANT)" $(CSANT_FLAGS) -f Testruntime.build all

CLEANFILES = Testruntime.dll

TESTS = Testruntime.dll
TESTS_ENVIRONMENT = $(SHELL) $(top_builddir)/tools/run_test.sh $(top_builddir)
