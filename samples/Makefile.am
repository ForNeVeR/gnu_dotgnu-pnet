## Build the example programs.

noinst_DATA = evenodd.exe hello.exe

EXTRA_DIST = evenodd.il hello.il

evenodd.exe: evenodd.il
	$(ILASM) -o evenodd.exe $(srcdir)/evenodd.il

hello.exe: hello.il
	$(ILASM) -o hello.exe $(srcdir)/hello.il

CLEANFILES = $(noinst_DATA)
