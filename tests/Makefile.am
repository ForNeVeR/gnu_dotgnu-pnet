
all-local:
	"$(CSANT)" $(CSANT_FLAGS) -Dmono.sources="$(MONO_SOURCE_PATH)" \
			-Dpnet.libdir="$(PNETLIB_PATH)" \
			-b "$(srcdir)" -B . -f "$(srcdir)/Test.build" all

CLEANFILES = Testcorlib.dll TestSystem.Xml.dll \
		TestSystem.Runtime.Serialization.Formatters.Soap.dll \
		TestSystem.Security.dll TestMono.Security.dll TestSystem.Data.dll \
		TestSystem.Web.Services.dll TestSystem.Runtime.Remoting.dll \
		TestMono.Directory.LDAP.dll

TESTS = Testcorlib.dll TestSystem.Xml.dll \
		TestSystem.Runtime.Serialization.Formatters.Soap.dll \
		TestSystem.Security.dll TestMono.Security.dll TestSystem.Data.dll \
		TestSystem.Web.Services.dll TestSystem.Runtime.Remoting.dll \
		TestMono.Directory.LDAP.dll
TESTS_ENVIRONMENT = $(SHELL) $(top_builddir)/tools/run_test.sh $(top_builddir)
