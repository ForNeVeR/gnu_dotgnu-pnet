bin_PROGRAMS     = ilnative
noinst_PROGRAMS  = ilinternal ilcheck
man_MANS         = ilnative.1
EXTRA_DIST       = $(man_MANS)

ilnative_SOURCES = ilnative.c
ilnative_LDADD   = ../dumpasm/libILDumpAsm.a ../image/libILImage.a \
				   ../support/libILSupport.a

ilcheck_SOURCES = ilcheck.c
ilcheck_LDADD = ../engine/libILEngine.a ../dumpasm/libILDumpAsm.a \
			  ../image/libILImage.a ../support/libILSupport.a \
			  ../libffi/.libs/libffi.a ../libgc/.libs/libgc.a

ilinternal_SOURCES = ilinternal.c
ilinternal_LDADD   = ../image/libILImage.a ../support/libILSupport.a

if USINGGCC
INCLUDES = -Wall
endif
