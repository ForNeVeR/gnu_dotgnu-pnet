bin_PROGRAMS = ilrun ilverify
lib_LIBRARIES = libILEngine.a

noinst_PROGRAMS = ilrun_opt
noinst_LIBRARIES = libILEngineOpt.a

COMMON_ENGINE_SOURCES = call.c \
						convert.c \
						cvm_dasm.c \
						cvmc.c \
						heap.c \
						internal.c \
						layout.c \
						lib_array.c \
						lib_decimal.c \
						lib_diag.c \
						lib_helpers.c \
						lib_math.c \
						lib_object.c \
						lib_platform.c \
						lib_stdio.c \
						lib_string.c \
						lib_thread.c \
						lib_type.c \
						lookup.c \
						null_coder.c \
						pinvoke.c \
						process.c \
						thread.c \
						throw.c \
						verify.c

libILEngine_a_SOURCES = cvm.c \
						$(COMMON_ENGINE_SOURCES)

libILEngineOpt_a_SOURCES = cvm_opt.s \
						   $(COMMON_ENGINE_SOURCES)

ilrun_SOURCES = ilrun.c
ilrun_LDADD = libILEngine.a ../dumpasm/libILDumpAsm.a \
			  ../image/libILImage.a ../support/libILSupport.a \
			  ../libffi/libffi.a ../libgc/.libs/libgc.a

ilrun_opt_SOURCES = ilrun.c
ilrun_opt_LDADD = libILEngineOpt.a ../dumpasm/libILDumpAsm.a \
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

cvm_opt.s: cvm.c asmfix cvm.o
	$(COMPILE) -S -o cvm_opt.s.tmp $<
	./asmfix <cvm_opt.s.tmp >cvm_opt.s
	rm -f cvm_opt.s.tmp

asmfix: asmfix.c
	$(COMPILE) -o $@ $< ../support/libILSupport.a

CLEANFILES = cvm_opt.s cvm_opt.s.tmp asmfix
