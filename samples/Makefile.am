
AUTOMAKE_OPTIONS = no-dependencies

noinst_PROGRAMS = dirent.exe hello.exe hello2.exe stack.exe pwd.exe

dirent_exe_SOURCES = dirent.c
dirent_exe_LDADD = -L../libc -lOpenSystem.C -llibc$(LIBRARY_SUFFIX)

hello_exe_SOURCES = hello.c
hello_exe_LDADD = -L../libc -lOpenSystem.C -llibc$(LIBRARY_SUFFIX)

hello2_exe_SOURCES = hello2.c
hello2_exe_LDADD = -L../libc -lOpenSystem.C -llibc$(LIBRARY_SUFFIX)

stack_exe_SOURCES = stack.c
stack_exe_LDADD = -L../libc -lOpenSystem.C -llibc$(LIBRARY_SUFFIX)

pwd_exe_SOURCES = pwd.c
pwd_exe_LDADD = -L../libc -lOpenSystem.C -llibc$(LIBRARY_SUFFIX)

AM_CFLAGS = -I$(top_srcdir)/include \
			-imacros $(top_srcdir)/include/libc-symbols.h
