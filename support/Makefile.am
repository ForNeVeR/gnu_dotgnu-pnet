lib_LIBRARIES = libILSupport.a
noinst_PROGRAMS = mkcategory mknumber mkcase

libILSupport_a_SOURCES = aes.c \
						 allocate.c \
						 ansi.c \
						 bignum.c \
						 clflush.c \
						 cmdline.c \
						 console.c \
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
						 path.c \
						 pt_defs.c \
						 queue.c \
						 rc2.c \
						 read_float.c \
						 regex.c \
						 rem_float.c \
						 serial.c \
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
						 wait_event.c \
						 wakeup.c \
						 write_float.c \
						 xml.c

mkcategory_SOURCES = mkcategory.c

mknumber_SOURCES = mknumber.c

mkcase_SOURCES = mkcase.c

AM_CFLAGS = -I$(top_srcdir)/libgc/include -I$(top_srcdir)/include \
			-DCSCC_LIB_PREFIX=\"$(libdir)\" \
		    -DCSCC_BIN_PREFIX=\"$(bindir)\" \
		    -DCSCC_DATA_PREFIX=\"$(datadir)\" \
			-DCSCC_HOST_TRIPLET=\"$(host_triplet)\"

CLEANFILES = $(srcdir)/errno_map.c

$(srcdir)/errno_map.c: $(srcdir)/gen_errno.sh
	$(SHELL) $(srcdir)/gen_errno.sh ${top_srcdir}/include/il_errno.h \
		$(COMPILE) >$(srcdir)/errno_map.c
