bin_PROGRAMS     = ilranlib
man_MANS         = ilranlib.1
EXTRA_DIST       = $(man_MANS)
ilranlib_SOURCES = ilranlib.c
ilranlib_LDADD   = ../support/libILSupport.a

AM_CFLAGS = -I$(top_srcdir)/include

install-exec-local:
	@$(NORMALL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	rm -f $(DESTDIR)$(bindir)/cli-unknown-ranlib$(EXEEXT)
	rm -f $(DESTDIR)$(bindir)/cli-unknown-ar$(EXEEXT)
	$(LN_S) ilranlib$(EXEEXT) $(DESTDIR)$(bindir)/cli-unknown-ranlib$(EXEEXT)
	$(LN_S) $(SYSTEM_AR) $(DESTDIR)$(bindir)/cli-unknown-ar$(EXEEXT)

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(bindir)/cli-unknown-ranlib$(EXEEXT)
	rm -f $(DESTDIR)$(bindir)/cli-unknown-ar$(EXEEXT)
