bin_PROGRAMS = ilrun ilverify
lib_LIBRARIES = libILEngine.a

libILEngine_a_SOURCES = heap.c \
						interpreter.c \
						layout.c \
						process.c \
						thread.c \
						verify.c

ilrun_SOURCES = ilrun.c
ilrun_LDADD = libILEngine.a ../image/libILImage.a ../support/libILSupport.a \
			  ../libffi/libffi.a

ilverify_SOURCES = ilverify.c
ilverify_LDADD = libILEngine.a ../image/libILImage.a ../support/libILSupport.a \
			     ../libffi/libffi.a

if USINGGCC
INCLUDES = -Wall
endif
