
.PHONY: Microsoft.JScript.dll

all-local: Microsoft.JScript.dll

Microsoft.JScript.dll: Nodes/JNode.cs
	"$(CSANT)" $(CSANT_FLAGS) -f JScript.build all

TREECC_INPUTS = $(srcdir)/Nodes/JNode.tc \
				$(srcdir)/Nodes/JExpr.tc \
				$(srcdir)/Nodes/JStmt.tc

Nodes/JNode.cs: $(TREECC_INPUTS)
	$(TREECC) -f -o Nodes/JNode.cs $(srcdir)/Nodes/JNode.tc

CLEANFILES = Microsoft.JScript.dll JScript.Test.dll Nodes/JNode.cs jsrun.exe

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = Microsoft.JScript.dll

install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) jsrun.exe $(DESTDIR)$(bindir)/jsrun.exe
	$(INSTALL_SCRIPT) $(srcdir)/jsrun.sh $(DESTDIR)$(bindir)/jsrun

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(bindir)/jsrun.exe
	rm -f $(DESTDIR)$(bindir)/jsrun
