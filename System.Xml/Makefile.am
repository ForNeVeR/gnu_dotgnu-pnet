
.PHONY: System.Xml.dll

all-local: System.Xml.dll

System.Xml.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f Xml.build all

CLEANFILES = System.Xml.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f Xml.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f Xml.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all

