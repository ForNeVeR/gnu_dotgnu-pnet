bin_PROGRAMS   = resgen
man_MANS       = resgen.1
EXTRA_DIST     = $(man_MANS)
resgen_SOURCES = resgen.c
resgen_LDADD   = ../image/libILImage.a ../support/libILSupport.a

if USINGGCC
INCLUDES = -Wall
endif
