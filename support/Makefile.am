lib_LIBRARIES = libILSupport.a
noinst_PROGRAMS = mkcategory mknumber

libILSupport_a_SOURCES = alloc.c \
						 cmdline.c \
						 decimal.c \
						 file.c \
						 filemap.c \
						 guid.c \
						 hash.c \
						 hashtab.c \
						 intern.c \
						 memory.c \
						 mempool.c \
						 memstack.c \
						 read_float.c \
						 spawn.c \
						 strings.c \
						 time.c \
						 unicode.c \
						 utf8.c \
						 value.c \
						 write_float.c \
						 xml.c

mkcategory_SOURCES = mkcategory.c

mknumber_SOURCES = mknumber.c

if USINGGCC
INCLUDES = -Wall
endif
