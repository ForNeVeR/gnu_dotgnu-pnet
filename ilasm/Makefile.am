bin_PROGRAMS  = ilasm
man_MANS      = ilasm.1
EXTRA_DIST    = $(man_MANS)
ilasm_SOURCES = ilasm_build.c \
				ilasm_data.c \
				ilasm_grammar.y \
				ilasm_main.c \
				ilasm_output.c \
			    ilasm_scanner.l
ilasm_LDADD   = ../image/libILImage.a ../support/libILSupport.a

AM_YFLAGS = -d
CLEANFILES = ilasm_grammar.c ilasm_scanner.c ilasm_grammar.h

AM_CFLAGS = -I$(top_srcdir)/include

## Install a second copy of "ilasm" into the "${prefix}/lib/cscc/plugins"
## directory, just in case some other package overwrites our primary
## copy with its own "ilasm".

install-exec-local:
	@$(NORMALL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/plugins
	$(INSTALL_PROGRAM) ilasm $(DESTDIR)$(libdir)/cscc/plugins/ilasm

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(libdir)/cscc/plugins/ilasm
