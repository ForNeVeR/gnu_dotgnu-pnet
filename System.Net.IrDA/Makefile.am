
.PHONY: System.Net.IrDA.dll

all-local: System.Net.IrDA.dll

System.Net.IrDA.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f IrDA.build all

CLEANFILES = System.Net.IrDA.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f IrDA.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f IrDA.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all

