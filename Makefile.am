SUBDIRS = . include support image dumpasm engine ilasm ildasm \
		  ilalink ilsize ilnative ilheader ilfind ildiff ildd codegen cscc \
		  resgen ildb csdoc csant clrwrap coremul samples doc tests

## We build libffi and libgc carefully, because we need to be
## able to turn them off in "configure".

.PHONY: libffi-all-local libgc-all-local \
		libffi-clean-local libgc-clean-local \
		libffi-distclean-local libgc-distclean-local \
		libffi-install-local libgc-install-local \
		libffi-uninstall-local libgc-uninstall-local

all-local: libffi-all-local libgc-all-local
clean-local: libffi-clean-local libgc-clean-local
distclean-local: libffi-distclean-local libgc-distclean-local
install-exec-local: libffi-install-local libgc-install-local
uninstall-local: libffi-uninstall-local libgc-uninstall-local

if USING_LIBFFI

libffi-all-local:
	(cd libffi && $(MAKE) $(AM_MAKEFLAGS) all)

libffi-clean-local:
	(cd libffi && $(MAKE) $(AM_MAKEFLAGS) clean)

libffi-distclean-local:
	(cd libffi && $(MAKE) $(AM_MAKEFLAGS) distclean)

libffi-install-local:
	(cd libffi && $(MAKE) $(AM_MAKEFLAGS) install)

libffi-uninstall-local:
	(cd libffi && $(MAKE) $(AM_MAKEFLAGS) uninstall)

else

libffi-all-local:
libffi-clean-local:
libffi-distclean-local:
libffi-install-local:
libffi-uninstall-local:

endif

if USING_LIBGC

libgc-all-local:
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) all)

libgc-clean-local:
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) clean)

libgc-distclean-local:
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) distclean)

libgc-install-local:
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) install)

libgc-uninstall-local:
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) uninstall)

else

libgc-all-local:
libgc-clean-local:
libgc-distclean-local:
libgc-install-local:
libgc-uninstall-local:

endif
