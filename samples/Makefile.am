noinst_DATA = evenodd.exe hello.exe

evenodd.exe: evenodd.il
	../ilasm/ilasm -o evenodd.exe evenodd.il

hello.exe: hello.il
	../ilasm/ilasm -o hello.exe hello.il

CLEANFILES = $(noinst_DATA)
