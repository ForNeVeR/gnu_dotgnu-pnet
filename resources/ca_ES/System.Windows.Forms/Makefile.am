
.PHONY: System.Windows.Forms.resources.dll

all-local: System.Windows.Forms.resources.dll

System.Windows.Forms.resources.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build all

install-data-local:
	@$(NORMAL_INSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all

CLEANFILES = System.Windows.Forms.ca.resources \
			 System.Windows.Forms.resources.dll
