bin_PROGRAMS   = ildiff
man_MANS       = ildiff.1
EXTRA_DIST     = $(man_MANS)

ildiff_SOURCES = ildiff.c pretty.h
ildiff_LDADD   = ../dumpasm/libILDumpAsm.a ../image/libILImage.a \
				 ../support/libILSupport.a
