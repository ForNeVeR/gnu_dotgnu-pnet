bin_PROGRAMS     = ilranlib
man_MANS         = ilranlib.1
EXTRA_DIST       = $(man_MANS)
ilranlib_SOURCES = ilranlib.c
ilranlib_LDADD   = ../support/libILSupport.a

AM_CFLAGS = -I$(top_srcdir)/include

install-exec-local:
	@$(NORMALL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	rm -f $(DESTDIR)$(bindir)/cli-unknown-ranlib
	$(LN_S) ilranlib $(DESTDIR)$(bindir)/cli-unknown-ranlib

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(bindir)/cli-unknown-ranlib
