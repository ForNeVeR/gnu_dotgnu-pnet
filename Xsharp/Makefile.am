
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

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Xsharp.dll
