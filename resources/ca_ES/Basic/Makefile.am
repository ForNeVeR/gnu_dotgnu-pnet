
.PHONY: Microsoft.VisualBasic.resources.dll

all-local: Microsoft.VisualBasic.resources.dll

Microsoft.VisualBasic.resources.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build all

install-data-local:
	@$(NORMAL_INSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all

CLEANFILES = Microsoft.VisualBasic.ca.resources \
			 Microsoft.VisualBasic.resources.dll
