bin_PROGRAMS = ilrun ilverify
lib_LIBRARIES = libILEngine.a

libILEngine_a_SOURCES = call.c \
						convert.c \
						cvm.c \
						cvm_dasm.c \
						cvmc.c \
						heap.c \
						internal.c \
						layout.c \
						lib_array.c \
						lib_decimal.c \
						lib_helpers.c \
						lib_math.c \
						lib_object.c \
						lib_platform.c \
						lib_stdio.c \
						lib_string.c \
						lib_thread.c \
						lib_type.c \
						lookup.c \
						pinvoke.c \
						process.c \
						thread.c \
						throw.c \
						verify.c

ilrun_SOURCES = ilrun.c
ilrun_LDADD = libILEngine.a ../dumpasm/libILDumpAsm.a \
			  ../image/libILImage.a ../support/libILSupport.a \
			  ../libffi/libffi.a ../libgc/.libs/libgc.a

ilverify_SOURCES = ilverify.c
ilverify_LDADD = libILEngine.a ../dumpasm/libILDumpAsm.a \
			     ../image/libILImage.a ../support/libILSupport.a \
			     ../libffi/libffi.a ../libgc/.libs/libgc.a

if USINGGCC
INCLUDES = -Wall -I../libffi/include
else
INCLUDES = -I../libffi/include
endif
