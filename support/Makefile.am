lib_LIBRARIES = libILSupport.a
noinst_PROGRAMS = mkcategory mknumber

libILSupport_a_SOURCES = alloc.c \
						 cmdline.c \
						 convert.c \
						 decimal.c \
						 dynlib.c \
						 errno_map.c \
						 expand.c \
						 file.c \
						 filemap.c \
						 getcwd.c \
						 guid.c \
						 hash.c \
						 hashtab.c \
						 hb_gc.c \
						 intern.c \
						 md5.c \
						 memory.c \
						 mempool.c \
						 memstack.c \
						 mul_long.c \
						 read_float.c \
						 rem_float.c \
						 sha1.c \
						 spawn.c \
						 strings.c \
						 test_float.c \
						 time.c \
						 unicode.c \
						 utf8.c \
						 utf16.c \
						 write_float.c \
						 xml.c

mkcategory_SOURCES = mkcategory.c

mknumber_SOURCES = mknumber.c

if USINGGCC
INCLUDES = -Wall
endif

CLEANFILES = errno_map.c

errno_map.c: gen_errno.sh
	$(SHELL) gen_errno.sh ${top_srcdir}/include/il_errno.h $(COMPILE) >errno_map.c
