
.PHONY: System.Windows.Forms.dll

all-local: System.Windows.Forms.dll

System.Windows.Forms.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f Forms.build all

CLEANFILES = System.Windows.Forms.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f Forms.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f Forms.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
