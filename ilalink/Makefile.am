bin_PROGRAMS    = ilalink
lib_LIBRARIES   = libILLink.a
#man_MANS        = ilalink.1
#EXTRA_DIST      = $(man_MANS)

ilalink_SOURCES = ilalink.c
ilalink_LDADD   = libILLink.a ../dumpasm/libILDumpAsm.a \
				  ../image/libILImage.a ../support/libILSupport.a

libILLink_a_SOURCES = link_attrs.c \
					  link_class.c \
					  link_create.c \
					  link_field.c \
					  link_image.c \
					  link_library.c \
					  link_main.c \
					  link_method.c \
					  link_misc.c \
					  link_module.c \
					  link_res.c \
					  link_type.c

AM_CFLAGS = -I$(top_srcdir)/include

install-exec-hook:
	ln $(DESTDIR)$(bindir)/ilalink$(EXEEXT) $(DESTDIR)$(bindir)/al$(EXEEXT)
