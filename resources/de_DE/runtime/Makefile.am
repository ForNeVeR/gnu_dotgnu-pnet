SUFFIXES = .txt .resources

RESOURCEBASE     = runtime
RESOURCELANGUAGE = de
RESOURCEPARENT   = mscorlib

RESOURCENAME = $(RESOURCEBASE).$(RESOURCELANGUAGE).resources
RESOURCEDLL  = $(RESOURCEPARENT).resources.dll

RESOURCEFILES = Arg.txt \
				Cast.txt \
				Crypto.txt \
				DivZero.txt \
				Emit.txt \
				Exception.txt \
				Format.txt \
				Invalid.txt \
				IO.txt \
				NotSupp.txt \
				Overflow.txt \
				Reflection.txt \
				Security.txt \
				Serialize.txt

resassembliesdir = $(libdir)/cscc/lib/$(RESOURCELANGUAGE)
resassemblies_DATA = $(RESOURCEDLL)

$(RESOURCEDLL): $(RESOURCENAME)
	$(SHELL) "$(top_srcdir)/tools/link_res.sh" $(RESOURCEDLL) $(RESOURCENAME) $(RESOURCELANGUAGE)

CLEANFILES = $(RESOURCENAME) $(RESOURCEBASE).resources $(RESOURCEDLL) tempres.txt

$(RESOURCENAME): $(RESOURCEFILES)
	"$(RESGEN)" $(RESGEN_FLAGS) $(RESGEN_LATIN1) \
			$(RESOURCEFILES) $(RESOURCENAME)
