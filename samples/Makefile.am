## Build the example programs.

noinst_DATA = evenodd.exe hello.exe fib.exe getenv.exe except.exe \
			  codepage.exe httpsrv.exe

evenodd.exe: evenodd.il
	../ilasm/ilasm -o evenodd.exe evenodd.il

hello.exe: hello.il
	../ilasm/ilasm -o hello.exe hello.il

fib.exe: fib.cs
	../cscc/cscc -fplugin-cs-path=../cscc/cscc-cs \
		-filasm-path=../ilasm/ilasm -filalink-path=../ilalink/ilalink \
		-o fib.exe fib.cs

getenv.exe: getenv.cs
	../cscc/cscc -fplugin-cs-path=../cscc/cscc-cs \
		-filasm-path=../ilasm/ilasm -filalink-path=../ilalink/ilalink \
		-o getenv.exe getenv.cs

except.exe: except.cs
	../cscc/cscc -fplugin-cs-path=../cscc/cscc-cs \
		-filasm-path=../ilasm/ilasm -filalink-path=../ilalink/ilalink \
		-g -o except.exe except.cs
		
httpsrv.exe: httpsrv.cs
	../cscc/cscc -lSystem -fplugin-cs-path=../cscc/cscc-cs \
		-filasm-path=../ilasm/ilasm -filalink-path=../ilalink/ilalink \
		-o httpsrv.exe httpsrv.cs

codepage.exe: codepage.cs
	../cscc/cscc -fplugin-cs-path=../cscc/cscc-cs \
		-filasm-path=../ilasm/ilasm -filalink-path=../ilalink/ilalink \
		-o codepage.exe codepage.cs

CLEANFILES = $(noinst_DATA)

## Install the system assemblies in the correct directory.

pnetassembliesdir = $(libdir)/cscc/lib
pnetassemblies_DATA = mscorlib.dll \
					  I18N.CJK.dll \
					  I18N.dll \
					  I18N.MidEast.dll \
					  I18N.Other.dll \
					  I18N.Rare.dll \
					  I18N.West.dll \
					  I18N-handlers.def \
					  System.dll \
					  System.Xml.dll \
					  pnetlib.here
