noinst_DATA = evenodd.exe hello.exe fib.obj

evenodd.exe: evenodd.il
	../ilasm/ilasm -o evenodd.exe evenodd.il

hello.exe: hello.il
	../ilasm/ilasm -o hello.exe hello.il

fib.obj: fib.cs
	../cscc/cscc -fplugin-cs-path=../cscc/cscc-cs -filasm-path=../ilasm/ilasm -o fib.obj -c fib.cs

CLEANFILES = $(noinst_DATA)
