SUFFIXES = .txt .resources

RESOURCENAME = System.Drawing.resources

RESOURCEFILES = System.Drawing.txt

noinst_DATA = $(RESOURCENAME)

CLEANFILES = $(RESOURCENAME) tempres.txt

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
