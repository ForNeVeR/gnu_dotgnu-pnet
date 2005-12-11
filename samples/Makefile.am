
noinst_PROGRAMS = test

test_SOURCES = test.c
test_LDADD = $(top_builddir)/src/libCrayons.la
test_DEPENDENCIES = $(top_builddir)/src/libCrayons.la

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I. -I$(srcdir)
AM_CXXFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I. -I$(srcdir)

