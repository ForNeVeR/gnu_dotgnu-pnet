bin_PROGRAMS   = ilsize
man_MANS       = ilsize.1
EXTRA_DIST     = $(man_MANS)
ilsize_SOURCES = ilsize.c
ilsize_LDADD   = ../image/libILImage.a ../support/libILSupport.a

if USINGGCC
INCLUDES = -Wall
endif
