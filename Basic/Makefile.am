
.PHONY: Microsoft.VisualBasic.dll

all-local: Microsoft.VisualBasic.dll

Microsoft.VisualBasic.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f Basic.build all

CLEANFILES = Microsoft.VisualBasic.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f Basic.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f Basic.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
