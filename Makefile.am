SUBDIRS = . include support image dumpasm engine ilasm ildasm \
		  ilalink ilsize ilnative ilfind ildiff codegen cscc resgen ildb \
		  csdoc csant samples doc tests

## We don't include libffi and libgc in SUBDIRS because we don't
## want "make install" to install those packages.

.PHONY: libffi-all-local libgc-all-local \
		libffi-clean-local libgc-clean-local \
		libffi-distclean-local libgc-distclean-local

all-local: libffi-all-local libgc-all-local
clean-local: libffi-clean-local libgc-clean-local
distclean-local: libffi-distclean-local libgc-distclean-local

if USING_LIBFFI

libffi-all-local:
	(cd libffi && $(MAKE) $(AM_MAKEFLAGS) all)

libffi-clean-local:
	(cd libffi && $(MAKE) $(AM_MAKEFLAGS) clean)

libffi-distclean-local:
	(cd libffi && $(MAKE) $(AM_MAKEFLAGS) distclean)

else

libffi-all-local:
libffi-clean-local:
libffi-distclean-local:

endif

if USING_LIBGC

libgc-all-local:
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) all)

libgc-clean-local:
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) clean)

libgc-distclean-local:
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) distclean)

else

libgc-all-local:
libgc-clean-local:
libgc-distclean-local:

endif
