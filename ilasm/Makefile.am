bin_PROGRAMS  = ilasm
man_MANS      = ilasm.1
EXTRA_DIST    = $(man_MANS)
ilasm_SOURCES = ilasm_build.c \
				ilasm_data.c \
				ilasm_grammar.y \
				ilasm_main.c \
				ilasm_output.c \
			    ilasm_scanner.l
ilasm_LDADD   = ../image/libILImage.a ../support/libILSupport.a

YFLAGS = -d
CLEANFILES = ilasm_grammar.c ilasm_scanner.c ilasm_grammar.h

if USINGGCC
INCLUDES = -Wall
endif
