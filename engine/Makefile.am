bin_PROGRAMS = ilrun ilverify
lib_LIBRARIES = libILEngine.a

libILEngine_a_SOURCES = call.c \
						convert.c \
						cvm.c \
						cvmc.c \
						heap.c \
						layout.c \
						lookup.c \
						pinvoke.c \
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
INCLUDES = -Wall -I../libffi/include
else
INCLUDES = -I../libffi/include
endif
