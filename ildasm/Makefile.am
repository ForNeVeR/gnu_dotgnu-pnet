bin_PROGRAMS   = ildasm
man_MANS       = ildasm.1
EXTRA_DIST     = $(man_MANS)
ildasm_SOURCES = ildasm_attrs.c \
				 ildasm_class.c \
				 ildasm_data.c \
				 ildasm_global.c \
				 ildasm_internal.h \
				 ildasm_java.c \
				 ildasm_main.c \
				 ildasm_method.c \
				 ildasm_utils.c
ildasm_LDADD   = ../dumpasm/libILDumpAsm.a ../image/libILImage.a \
				 ../support/libILSupport.a $(GCLIBS)

AM_CFLAGS = -I$(top_srcdir)/include
