
all-local:
	"$(CSANT)" $(CSANT_FLAGS) -Dpnet.cstest.library="$(CSTEST_LIBRARY)" \
			-f TestSystem.build all

CLEANFILES = TestSystem.dll

TESTS = TestSystem.dll
TESTS_ENVIRONMENT = $(SHELL) $(top_builddir)/tools/run_test.sh
