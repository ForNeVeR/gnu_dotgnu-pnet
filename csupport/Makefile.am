
clangdir   = $(libdir)/cscc/lib
clang_DATA = OpenSystem.C.dll

CSSRC = $(srcdir)/Crt0.cs \
		$(srcdir)/FileTable.cs \
		$(srcdir)/LongJmp.cs \
		$(srcdir)/Markers.cs

CSCC = ../cscc/cscc

CSFLAGS	= -L$(top_srcdir)/samples -fplugin-cs-path=../cscc/cscc-cs \
		  -filasm-path=../ilasm/ilasm -funsafe

OpenSystem.C.dll: $(CSSRC)
	$(CSCC) $(CSFLAGS) -o $@ $(CSSRC)

CLEANFILES = $(clang_DATA)
