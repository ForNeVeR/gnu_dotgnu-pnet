
.PHONY: samples

all-local: samples

samples:
	"$(CSANT)" $(CSANT_FLAGS) -b "$(srcdir)" -B . \
			-f "$(srcdir)/samples.build" all

CLEANFILES = *.exe
