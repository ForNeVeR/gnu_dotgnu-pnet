noinst_PROGRAMS = test_verify

test_verify_SOURCES = test_verify.c ilunit.c
test_verify_LDADD   = ../engine/libILEngine.a ../dumpasm/libILDumpAsm.a \
			  		  ../image/libILImage.a ../support/libILSupport.a \
			  		  ../libffi/libffi.a ../libgc/.libs/libgc.a

if USINGGCC
INCLUDES = -Wall
endif

TESTS = test_verify
