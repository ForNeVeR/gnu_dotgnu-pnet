bin_PROGRAMS  = ilasm
lib_LIBRARIES = libILAsm.a
man_MANS      = ilasm.1
EXTRA_DIST    = $(man_MANS)

libILAsm_a_SOURCES = ilasm_build.c \
					 ilasm_data.c \
					 ilasm_grammar.y \
					 ilasm_main.c \
					 ilasm_output.c \
			    	 ilasm_scanner.l

ilasm_SOURCES = ilasm.c
ilasm_LDADD   = libILAsm.a ../image/libILImage.a ../support/libILSupport.a

AM_YFLAGS = -d
CLEANFILES = ilasm_grammar.c ilasm_scanner.c ilasm_grammar.h

AM_CFLAGS = -I$(top_srcdir)/include
