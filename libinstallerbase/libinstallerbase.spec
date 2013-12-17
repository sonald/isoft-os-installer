Name:	libinstallerbase
Version:	0.1
Release:    1
Summary:	Base libraries of cetcosinstaller
License:	GPL
Group:	Applications/System
Requires:	parted
BuildRoot : /var/tmp/%{name}-%{version}-root
Source:	%{name}-%{version}.tar.bz2
%description
Base libraries of cetcosinstaller, including libparted++, libinstallengine

%prep
%setup -q

%build
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(-,root,root,-)
#%{_bindir}/cetcosinstaller_console
%{_libdir}/libinstallengine.so
%{_libdir}/libparted++.so
%{_datadir}/apps/libinstallerbase/*
%{_includedir}/installengine.h
%{_includedir}/gen_grub_conf.h
%{_includedir}/yumshell.h
%{_includedir}/parted++/*
%{_includedir}/os_identify.h


%changelog
* Tue Dec 17 2013 Sian Cao <siyuan.cao@i-soft.com> 0.1-1
- init build

