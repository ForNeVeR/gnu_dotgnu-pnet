
all-local:
	"$(CSANT)" $(CSANT_FLAGS) -f TestSystem.build all

CLEANFILES = TestSystem.dll

TESTS = TestSystem.dll
TESTS_ENVIRONMENT = $(SHELL) $(top_builddir)/tools/run_test.sh $(top_builddir)
