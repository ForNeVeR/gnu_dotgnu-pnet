SUFFIXES = .txt .resources

RESOURCEBASE     = System
RESOURCELANGUAGE = de
RESOURCEPARENT   = System

RESOURCENAME = $(RESOURCEBASE).$(RESOURCELANGUAGE).resources
RESOURCEDLL  = $(RESOURCEPARENT).resources.dll

RESOURCEFILES = Arg.txt \
				Exception.txt \
				Invalid.txt \
				IO.txt

resassembliesdir = $(libdir)/cscc/lib/$(RESOURCELANGUAGE)
resassemblies_DATA = $(RESOURCEDLL)

$(RESOURCEDLL): $(RESOURCENAME)
	"$(top_srcdir)/tools/link_res.sh" $(RESOURCEDLL) $(RESOURCENAME) $(RESOURCELANGUAGE)

CLEANFILES = $(RESOURCENAME) $(RESOURCEBASE).resources $(RESOURCEDLL) tempres.txt

if PNET_RESGEN

# Portable.NET's resgen can convert multiple input files.

$(RESOURCENAME): $(RESOURCEFILES)
	"$(RESGEN)" $(RESGEN_FLAGS) $(RESOURCEFILES) $(RESOURCENAME)

else

# Microsoft's resgen can only convert a single input file at a time,
# so we combine everything into one file and then convert.

$(RESOURCENAME): $(RESOURCEFILES)
	cat $(RESOURCEFILES) >tempres.txt
	"$(RESGEN)" $(RESGEN_FLAGS) tempres.txt $(RESOURCENAME)
	rm -f tempres.txt

endif
