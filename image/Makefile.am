lib_LIBRARIES = libILImage.a
noinst_LIBRARIES = libILTestMacros.a

libILImage_a_SOURCES = assembly.c \
					   attr.c \
					   class.c \
					   compact.c \
					   compress.c \
					   context.c \
					   create.c \
					   debug_reader.c \
					   debug_writer.c \
					   errors.c \
					   image.c \
					   item.c \
					   java_loader.c \
					   jopcodes.c \
					   link.c \
					   marshal.c \
					   member.c \
					   meta_build.c \
					   meta_header.c \
					   meta_index.c \
					   meta_writer.c \
					   misc.c \
					   module.c \
					   opcodes.c \
					   pecoff_loader.c \
					   pecoff_writer.c \
					   ser_parse.c \
					   sig_parse.c \
					   sig_writer.c \
					   synthetic.c \
					   types.c \
					   uncompress.c \
					   writer.c

libILTestMacros_a_SOURCES = test_macros.c

if USINGGCC
INCLUDES = -Wall -DCSCC_LIB_DIR=\"$(libdir)\" \
		   -DCSCC_HOST_TRIPLET=\"$(host_triplet)\" \
		   -DCSCC_HOST_ALIAS=\"$(host_alias)\"
else
INCLUDES = -DCSCC_LIB_DIR=\"$(libdir)\" \
		   -DCSCC_HOST_TRIPLET=\"$(host_triplet)\" \
		   -DCSCC_HOST_ALIAS=\"$(host_alias)\"
endif
