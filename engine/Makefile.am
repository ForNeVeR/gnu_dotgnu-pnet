bin_PROGRAMS = ilrun ilverify
lib_LIBRARIES = libILEngine.a

noinst_PROGRAMS = ilrun_opt
noinst_LIBRARIES = libILEngineOpt.a

COMMON_ENGINE_SOURCES = box.c \
						call.c \
						convert.c \
						cvm_dasm.c \
						cvmc.c \
						heap.c \
						internal.c \
						layout.c \
						lib_array.c \
						lib_decimal.c \
						lib_diag.c \
						lib_emit.c \
						lib_encoding.c \
						lib_enum.c \
						lib_file.c \
						lib_gc.c \
						lib_helpers.c \
						lib_math.c \
						lib_misc.c \
						lib_object.c \
						lib_platform.c \
						lib_reflect.c \
						lib_security.c \
						lib_socket.c \
						lib_stdio.c \
						lib_string.c \
						lib_thread.c \
						lib_type.c \
						lookup.c \
						method_cache.c \
						null_coder.c \
						pinvoke.c \
						process.c \
						register.c \
						thread.c \
						throw.c \
						verify.c

libILEngine_a_SOURCES = cvm.c \
						$(COMMON_ENGINE_SOURCES)

libILEngineOpt_a_SOURCES = cvm_opt.s \
						   $(COMMON_ENGINE_SOURCES)

PACKAGE_LIBS = ../libffi/.libs/libffi.a ../libgc/.libs/libgc.a

ilrun_SOURCES = ilrun.c
ilrun_LDADD = libILEngine.a ../dumpasm/libILDumpAsm.a \
			  ../image/libILImage.a ../support/libILSupport.a \
			  $(PACKAGE_LIBS)

ilrun_opt_SOURCES = ilrun.c
ilrun_opt_LDADD = libILEngineOpt.a ../dumpasm/libILDumpAsm.a \
			  ../image/libILImage.a ../support/libILSupport.a \
			  $(PACKAGE_LIBS)

ilverify_SOURCES = ilverify.c
ilverify_LDADD = libILEngine.a ../dumpasm/libILDumpAsm.a \
			     ../image/libILImage.a ../support/libILSupport.a \
			  	 $(PACKAGE_LIBS)

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

cvm.o: cvm_labels.h

cvm_labels.h: cvm.h mklabel.sh
	$(SHELL) mklabel.sh "$(AWK)" cvm.h >cvm_labels.h

CLEANFILES = cvm_opt.s cvm_opt.s.tmp asmfix cvm_labels.h
