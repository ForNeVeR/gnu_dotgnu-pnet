bin_PROGRAMS = ilrun
lib_LIBRARIES = libILEngine.a

libILEngine_a_SOURCES = heap.c \
						interpreter.c \
						layout.c \
						process.c \
						thread.c

ilrun_SOURCES = ilrun.c
ilrun_LDADD = libILEngine.a ../image/libILImage.a ../support/libILSupport.a \
			  ../libffi/libffi.a

if USINGGCC
INCLUDES = -Wall
endif
