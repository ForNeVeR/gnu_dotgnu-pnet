
.PHONY: System.Drawing.resources

all-local: System.Drawing.resources

System.Drawing.resources:
	"$(CSANT)" $(CSANT_FLAGS) -b "$(srcdir)" -B . \
			-f "$(srcdir)/resources.build" all

CLEANFILES = System.Drawing.resources
