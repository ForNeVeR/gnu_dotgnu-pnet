bin_PROGRAMS   = ilsize
man_MANS       = ilsize.1
EXTRA_DIST     = $(man_MANS)
ilsize_SOURCES = ilsize.c ilsize_est.c
ilsize_LDADD   = ../dumpasm/libILDumpAsm.a \
				 ../image/libILImage.a ../support/libILSupport.a

AM_CFLAGS = -I$(top_srcdir)/include

install-exec-local:
	@$(NORMALL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	rm -f $(DESTDIR)$(bindir)/cli-unknown-size
	$(LN_S) ilsize $(DESTDIR)$(bindir)/cli-unknown-size

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(bindir)/cli-unknown-size
