bin_PROGRAMS    = ilstrip
man_MANS        = ilstrip.1
EXTRA_DIST      = $(man_MANS)
ilstrip_SOURCES = ilstrip.c
ilstrip_LDADD   = ../support/libILSupport.a

AM_CFLAGS = -I$(top_srcdir)/include

install-exec-local:
	@$(NORMALL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	rm -f $(DESTDIR)$(bindir)/cli-unknown-strip$(EXEEXT)
	$(LN_S) ilstrip$(EXEEXT) $(DESTDIR)$(bindir)/cli-unknown-strip$(EXEEXT)

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(bindir)/cli-unknown-strip$(EXEEXT)
