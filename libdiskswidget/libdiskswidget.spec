Summary:	A common widget for cetc os installer
Name:		libdiskswidget
Version:	0.1
Release:    1
License:	GPL
Group:		Development/Libraries
Source:		%{name}-%{version}.tar.bz2
BuildRequires:	glib2, libinstallerbase
Requires:	glib2, libinstallerbase
BuildRoot:	/var/tmp/%{name}-%{version}-root
%description
A common widget used by Installer 

%prep
%setup 
#%setup -n %{name}

%build
lrelease *.ts
qmake
make %{?_smp_mflags}

%install
make INSTALL_ROOT=$RPM_BUILD_ROOT install

%files
/usr/include/*.h
/usr/lib/libdiskswidget.so*
/usr/share/libdiskswidget/

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%changelog
* Tue Dec 17 2013 Sian Cao <siyuan.cao@i-soft.com> 0.1-1
- init build
