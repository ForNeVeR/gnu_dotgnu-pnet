bin_PROGRAMS     = ilnative
man_MANS         = ilnative.1
EXTRA_DIST       = $(man_MANS)
ilnative_SOURCES = ilnative.c
ilnative_LDADD   = ../dumpasm/libILDumpAsm.a ../image/libILImage.a \
				   ../support/libILSupport.a

if USINGGCC
INCLUDES = -Wall
endif
