if HAS_REFLECTION
if HAS_EXTENDED_NUMERICS
SUBDIRS = tools config resources runtime I18N OpenSystem.Platform \
		  DotGNU.SSL System System.Xml . \
		  System.Net.IrDA csunit csupport compat ISymWrapper \
		  SharpZipLib System.Configuration.Install JScript Basic \
		  DotGNU.Images Xsharp System.Drawing System.Drawing.Xsharp \
		  System.Drawing.Win32 System.Drawing.Postscript \
		  System.EnterpriseServices System.Windows.Forms \
		  System.Design DotGNU.Terminal DotGNU.XmlRpc \
		  tests samples 
else
SUBDIRS = tools config resources runtime I18N OpenSystem.Platform \
		  DotGNU.SSL System System.Xml . \
		  System.Net.IrDA csunit csupport compat ISymWrapper \
		  SharpZipLib System.Configuration.Install Basic \
		  DotGNU.Images Xsharp System.EnterpriseServices \
		  DotGNU.Terminal DotGNU.XmlRpc \
		  tests samples
endif
else
SUBDIRS = tools resources runtime OpenSystem.Platform \
		  DotGNU.SSL System System.Xml . System.Net.IrDA samples
endif

## System.dll is built in two phases, because it depends recursively
## on System.Xml.dll.  The following takes care of the second phase.

all-local:
	(cd System; $(MAKE) $(AM_MAKEFLAGS) phase-two)
