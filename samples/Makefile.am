## Build the example programs.

noinst_DATA = evenodd.exe hello.exe fib.exe getenv.exe except.exe \
			  codepage.exe httpsrv.exe

CSCC	= ../cscc/cscc

## the -L flag is necessary to resolve mscorlib
CSFLAGS	= -L$(srcdir) -fplugin-cs-path=../cscc/cscc-cs \
		  -filasm-path=../ilasm/ilasm

evenodd.exe: evenodd.il
	../ilasm/ilasm -o evenodd.exe $(srcdir)/evenodd.il

hello.exe: hello.il
	../ilasm/ilasm -o hello.exe $(srcdir)/hello.il

fib.exe: fib.cs
	$(CSCC) $(CSFLAGS) -o fib.exe $(srcdir)/fib.cs

getenv.exe: getenv.cs
	$(CSCC) $(CSFLAGS) -o getenv.exe $(srcdir)/getenv.cs

except.exe: except.cs
	$(CSCC) $(CSFLAGS) -g -o except.exe $(srcdir)/except.cs

httpsrv.exe: httpsrv.cs
	$(CSCC) $(CSFLAGS) -lSystem -o httpsrv.exe $(srcdir)/httpsrv.cs

codepage.exe: codepage.cs
	$(CSCC) $(CSFLAGS) -o codepage.exe $(srcdir)/codepage.cs

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
