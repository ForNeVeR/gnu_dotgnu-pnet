bin_PROGRAMS     = ilnative ilcheck
man_MANS         = ilnative.1
EXTRA_DIST       = $(man_MANS)
ilnative_SOURCES = ilnative.c
ilnative_LDADD   = ../dumpasm/libILDumpAsm.a ../image/libILImage.a \
				   ../support/libILSupport.a

ilcheck_SOURCES = ilcheck.c
ilcheck_LDADD = ../engine/libILEngine.a ../dumpasm/libILDumpAsm.a \
			  ../image/libILImage.a ../support/libILSupport.a \
			  ../libffi/libffi.a ../libgc/.libs/libgc.a

if USINGGCC
INCLUDES = -Wall
endif
