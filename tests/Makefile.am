noinst_PROGRAMS = test_verify test_thread

test_verify_SOURCES = test_verify.c \
					  test_varith.c \
					  test_vconv.c \
					  test_vcompare.c \
					  test_vbranch.c \
					  test_vconst.c \
					  test_vlocal.c \
					  test_vmisc.c \
					  ilunit.c
test_verify_LDADD   = ../engine/libILEngine.a ../dumpasm/libILDumpAsm.a \
			  		  ../image/libILImage.a ../support/libILSupport.a \
			  		  $(FFILIBS) $(GCLIBS)

test_thread_SOURCES = test_thread.c \
					  ilunit.c
test_thread_LDADD   = ../image/libILImage.a ../support/libILSupport.a \
			  		  $(GCLIBS)

TESTS = test_verify test_thread

CFLAGS =
