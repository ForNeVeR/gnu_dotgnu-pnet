SUBDIRS = . include support image dumpasm engine ilasm ildasm \
		  ilalink ilsize ilnative ilfind ildiff codegen cscc resgen ildb \
		  csdoc csant samples doc tests

## We don't include libffi and libgc in SUBDIRS because we don't
## want "make install" to install those packages.

if USING_LIBFFI

all-local:
	(cd libffi && $(MAKE) $(AM_MAKEFLAGS) all)
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) all)

clean-local:
	(cd libffi && $(MAKE) $(AM_MAKEFLAGS) clean)
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) clean)

distclean-local:
	(cd libffi && $(MAKE) $(AM_MAKEFLAGS) distclean)
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) distclean)

else

all-local:
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) all)

clean-local:
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) clean)

distclean-local:
	(cd libgc && $(MAKE) $(AM_MAKEFLAGS) distclean)

endif
