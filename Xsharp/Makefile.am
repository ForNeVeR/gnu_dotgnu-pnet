
.PHONY: Xsharp.dll

xsupportdir = $(libdir)/cscc/lib
xsupport_LTLIBRARIES = libXsharpSupport.la

libXsharpSupport_la_SOURCES = XsharpSupport.c
libXsharpSupport_la_LDFLAGS = -version-info 0:0:0 -no-undefined \
							  $(X_LIBS) $(X_PRE_LIBS) $(XFT_LIB) \
							  $(X11_LIB) $(X_EXTRA_LIBS)

AM_CFLAGS = $(X_CFLAGS)

all-local: Xsharp.dll

Xsharp.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f Xsharp.build all

CLEANFILES = Xsharp.dll
DISTCLEANFILES = Xlib.cs

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f Xsharp.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f Xsharp.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
