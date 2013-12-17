Summary : CETC OS Installer
Name : cetcosinstaller
Version : 0.1
License : GPL
Release : 1
URL : http://www.redflag-linux.com
Group : Applications/System
BuildRoot : /var/tmp/%{name}-%{version}-root
Requires : libinstallerbase libdiskswidget qt4
BuildRequires : libinstallerbase libdiskswidget qt4-devel
Source : %{name}-%{version}.tar.bz2

%description
CETC OS Installer

%prep
%setup -q

%build
qmake
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
make install INSTALL_ROOT=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_DIR/%{name}

%files
/usr/bin/cetcosinstaller
/usr/share/cetcosinstaller/translations/*
/usr/share/cetcosinstaller/images/*
/usr/share/applications/cetcosinstaller.desktop
/usr/lib/librfinsgame.so*
/usr/share/cetcosinstaller/svg/*
/etc/*.xml

%changelog
* Tue Dec 17 2013 Sian Cao <siyuan.cao@i-soft.com> 0.1-1
- init build

