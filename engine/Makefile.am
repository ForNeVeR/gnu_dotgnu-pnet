bin_PROGRAMS = ilrun ilverify
lib_LIBRARIES = libILEngine.a
man_MANS = ilrun.1

libILEngine_a_SOURCES = cvm.c \
						box.c \
						call.c \
						convert.c \
						cvm_dasm.c \
						cvm_lengths.c \
						cvmc.c \
						delegate.c \
						heap.c \
						internal.c \
						layout.c \
						lib_array.c \
						lib_charinfo.c \
						lib_decimal.c \
						lib_diag.c \
						lib_dir.c \
						lib_emit.c \
						lib_encoding.c \
						lib_enum.c \
						lib_file.c \
						lib_gc.c \
						lib_helpers.c \
						lib_math.c \
						lib_misc.c \
						lib_object.c \
						lib_reflect.c \
						lib_security.c \
						lib_socket.c \
						lib_stdio.c \
						lib_string.c \
						lib_task.c \
						lib_thread.c \
						lib_time.c \
						lib_type.c \
						lookup.c \
						method_cache.c \
						null_coder.c \
						pinvoke.c \
						process.c \
						register.c \
						thread.c \
						throw.c \
						unroll_x86.c \
						verify.c

PACKAGE_LIBS = $(FFILIBS) $(GCLIBS)

ilrun_SOURCES = ilrun.c
ilrun_LDADD = libILEngine.a ../dumpasm/libILDumpAsm.a \
			  ../image/libILImage.a ../support/libILSupport.a \
			  $(PACKAGE_LIBS)

ilverify_SOURCES = ilverify.c
ilverify_LDADD = libILEngine.a ../dumpasm/libILDumpAsm.a \
			     ../image/libILImage.a ../support/libILSupport.a \
			  	 $(PACKAGE_LIBS)

## We need "-fno-gcse" to stop gcc being overeager when it compiles
## the interpreter core in direct threading mode.
AM_CFLAGS = -I../libffi/include $(NO_GCSE)

cvm.o: cvm_labels.h

cvm_labels.h: cvm.h mklabel.sh
	$(SHELL) mklabel.sh "$(AWK)" cvm.h >cvm_labels.h

CLEANFILES = cvm_opt.s cvm_opt.s.tmp asmfix ilrun_opt cvm_labels.h
