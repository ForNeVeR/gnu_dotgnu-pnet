bin_PROGRAMS = ildb
#man_MANS     = ildb.1
#EXTRA_DIST   = $(man_MANS)
ildb_SOURCES = ildb_cmd.c \
			   ildb_context.c \
			   ildb_display.c \
			   ildb_errors.c \
			   ildb_input.c \
			   ildb_list.c \
			   ildb_main.c \
			   ildb_set.c \
			   ildb_show.c \
			   ildb_system.c
ildb_LDADD   = ../engine/libILEngine.a ../image/libILImage.a \
			   ../support/libILSupport.a ../libffi/libffi.a \
			   $(READLINELIBS)

if USINGGCC
INCLUDES = -Wall
endif
