bin_PROGRAMS    = ilalink
lib_LIBRARIES   = libILLink.a
#man_MANS        = ilalink.1
#EXTRA_DIST      = $(man_MANS)

ilalink_SOURCES = ilalink.c
ilalink_LDADD   = libILLink.a ../image/libILImage.a ../support/libILSupport.a

libILLink_a_SOURCES = link_create.c \
					  link_image.c \
					  link_library.c

if USINGGCC
INCLUDES = -Wall
endif
