
.PHONY: mscorlib.dll

all-local: mscorlib.dll

mscorlib.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f runtime.build all

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f runtime.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f runtime.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all

CLEANFILES = mscorlib.dll
