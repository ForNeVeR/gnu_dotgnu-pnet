noinst_DATA = evenodd.exe hello.exe fib.exe

evenodd.exe: evenodd.il
	../ilasm/ilasm -o evenodd.exe evenodd.il

hello.exe: hello.il
	../ilasm/ilasm -o hello.exe hello.il

fib.exe: fib.cs
	../cscc/cscc -fplugin-cs-path=../cscc/cscc-cs \
		-filasm-path=../ilasm/ilasm -filalink-path=../ilalink/ilalink \
		-o fib.exe fib.cs

CLEANFILES = $(noinst_DATA)
