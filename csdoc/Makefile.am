bin_PROGRAMS       = csdoc2texi csdoc2html csdocvalil
man_MANS           = csdoc2texi.1 csdoc2html.1
EXTRA_DIST         = $(man_MANS)

CSDOC_COMMON       = doc_destroy.c \
					 doc_load.c \
					 doc_main.c \
					 doc_sort.c

csdoc2texi_SOURCES = doc_texi.c $(CSDOC_COMMON)
csdoc2texi_LDADD   = ../support/libILSupport.a

csdoc2html_SOURCES = doc_html.c $(CSDOC_COMMON)
csdoc2html_LDADD   = ../support/libILSupport.a

csdocvalil_SOURCES = doc_valil.c $(CSDOC_COMMON)
csdocvalil_LDADD   = ../image/libILImage.a ../support/libILSupport.a

if USINGGCC
INCLUDES = -Wall
endif
