if HAS_REFLECTION
SUBDIRS = tools resources runtime I18N DotGNU.SSL System System.Xml . \
		  System.Net.IrDA csunit csupport compat JScript Basic \
		  DotGNU.Images Xsharp System.Drawing System.Drawing.Xsharp \
		  System.Drawing.Win32 System.Drawing.Postscript \
		  System.Windows.Forms tests samples
else
SUBDIRS = tools resources runtime DotGNU.SSL System System.Xml . \
		  System.Net.IrDA samples
endif

## System.dll is built in two phases, because it depends recursively
## on System.Xml.dll.  The following takes care of the second phase.

all-local:
	(cd System; $(MAKE) $(AM_MAKEFLAGS) phase-two)
