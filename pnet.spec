Name: pnet
Version: 0.1.2
Release: 1mdk
Source: http://www.southern-storm.com.au/download/%{name}-%{version}.tar.gz
License: GPL
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Group: Development/Compilers
BuildRequires: flex, bison, treecc
Summary: The DotGNU Portable.NET platform
%description
The goal of this project is to build a suite of open source tools
to build and execute .NET applications, including a C# compiler,
assembler, disassembler, and runtime engine.  The initial target
platform is Linux, with other platforms to follow in the future.

%prep
%setup -q
%build
%configure
make
%install
make DESTDIR=${RPM_BUILD_ROOT} install
%clean
if test "/" != "${RPM_BUILD_ROOT}"; then rm -rf ${RPM_BUILD_ROOT}; fi
%files
%defattr(-,root,root)
%doc AUTHORS COPYING INSTALL NEWS README doc samples
%{_bindir}
%{_mandir}
%{_includedir}/pnet
%{_libdir}
