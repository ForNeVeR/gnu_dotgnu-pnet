
all-local:
	"$(CSANT)" $(CSANT_FLAGS) -Dpnet.cstest.library="$(CSTEST_LIBRARY)" \
			-f TestXml.build all

CLEANFILES = TestXml.dll

TESTS = TestXml.dll
TESTS_ENVIRONMENT = $(SHELL) $(top_builddir)/tools/run_test.sh $(top_builddir)
