## Build the example programs.

noinst_DATA = evenodd.exe hello.exe

evenodd.exe: evenodd.il
	../ilasm/ilasm -o evenodd.exe $(srcdir)/evenodd.il

hello.exe: hello.il
	../ilasm/ilasm -o hello.exe $(srcdir)/hello.il

CLEANFILES = $(noinst_DATA)
