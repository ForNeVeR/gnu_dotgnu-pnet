bin_PROGRAMS   = ildiff
man_MANS       =
EXTRA_DIST     = $(man_MANS)
ildiff_SOURCES = ildiff.c pretty.h
ildiff_LDADD   = ../image/libILImage.a ../support/libILSupport.a ../dumpasm/libILDumpAsm.a

if USINGGCC
INCLUDES = -Wall -I../includes/
endif
