
.PHONY: System.Drawing.dll

all-local: System.Drawing.dll

System.Drawing.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.build all

CLEANFILES = System.Drawing.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
