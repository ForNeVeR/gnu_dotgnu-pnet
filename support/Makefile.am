lib_LIBRARIES = libILSupport.a
noinst_PROGRAMS = mkcategory mknumber

libILSupport_a_SOURCES = aes.c \
						 allocate.c \
						 ansi.c \
						 bignum.c \
						 clflush.c \
						 cmdline.c \
						 cvt_float.c \
						 decimal.c \
						 def_gc.c \
						 des.c \
						 dir.c \
						 dns.c \
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
						 locale.c \
						 md5.c \
						 memory.c \
						 mempool.c \
						 memstack.c \
						 mul_long.c \
						 mutex.c \
						 no_defs.c \
						 numformat.c\
						 pt_defs.c \
						 queue.c \
						 rc2.c \
						 read_float.c \
						 regexp.c \
						 rem_float.c \
						 sha1.c \
						 sha256.c \
						 sha384.c \
						 sha512.c \
						 socket.c \
						 spawn.c \
						 strings.c \
						 test_float.c \
						 thread.c \
						 time.c \
						 unicode.c \
						 utf8.c \
						 utf16.c \
						 w32_defs.c \
						 wait.c \
						 wait_mutex.c \
						 wakeup.c \
						 write_float.c \
						 xml.c

mkcategory_SOURCES = mkcategory.c

mknumber_SOURCES = mknumber.c

AM_CFLAGS = -I$(top_srcdir)/libgc/include -I$(top_srcdir)/include

CLEANFILES = $(srcdir)/errno_map.c

$(srcdir)/errno_map.c: $(srcdir)/gen_errno.sh
	$(SHELL) $(srcdir)/gen_errno.sh ${top_srcdir}/include/il_errno.h \
		$(COMPILE) >$(srcdir)/errno_map.c
