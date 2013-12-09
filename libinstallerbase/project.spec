Name:	libinstallerbase
Version:	0.1
Release: 190
Summary:	Base libraries of rfinstaller
License:	RedFlag License
Vendor:	RedFlag Linux
Url:	http://www.redflag-linux.com
Group:	Applications/System
Packager:	packagerroot
Requires:	parted, ntregedit
BuildRoot : /var/tmp/%{name}-%{version}-root
Source:	%{name}-%{version}.tar.bz2
%description
Base libraries of rfinstaller, including libparted++, libinstallengin
and rfinstall_console

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
%{_bindir}/rfinstaller_console
%{_libdir}/libinstallengine.so
%{_libdir}/libparted++.so
%{_datadir}/apps/libinstallerbase/*
%{_includedir}/installengine.h
%{_includedir}/gen_grub_conf.h
%{_includedir}/yumshell.h
%{_includedir}/parted++/*
%{_includedir}/os_identify.h


%changelog
* Sun Mar 22 2009 wppan <wppan@redflag-linux.com> 0.1-190 #r3308  
- change mv to cp -a

* Sun Mar 22 2009 wppan <wppan@redflag-linux.com> 0.1-190 #r3307  
- change mount all partition's position

* Sun Mar 22 2009 jianxing <jianxing@redflag-linux.com> 0.1-190 #r3306  
- modify the kprinter.desktop

* Fri Mar 20 2009 wppan <wppan@redflag-linux.com> 0.1-189 #r3303  
- wait for /dev/sdaX be created at most 20 s

* Fri Mar 20 2009 wppan <wppan@redflag-linux.com> 0.1-188 #r3288  
- vfat partiton uuid

* Thu Mar 19 2009 wppan <wppan@redflag-linux.com> 0.1-187 #r3280  
- for vfat uuid

* Thu Mar 19 2009 wppan <wppan@redflag-linux.com> 0.1-187 #r3278  
- change sth for uuid

* Wed Mar 11 2009 wppan <wppan@redflag-linux.com> 0.1-186 #r3208  
- BUG 10059 

* Thu Mar 05 2009 wppan <wppan@redflag-linux.com> 0.1-185 #r3184  
- set root color

* Fri Feb 27 2009 wppan <wppan@redflag-linux.com> 0.1-184 #r3153  
- correct konsole cursor problem

* Fri Feb 20 2009 leojiang <leojiang@redflag-linux.com> 0.1-183 #r3105  
- To edit the etc/sysconfig/network of the dest system.

* Wed Feb 18 2009 leojiang <leojiang@redflag-linux.com> 0.1-182 #r3097  
- correct the literal error.

* Mon Feb 16 2009 wppan <wppan@redflag-linux.com> 0.1-181 #r3075  
- add set_grub_args() 

* Mon Feb 16 2009 leojiang <leojiang@redflag-linux.com> 0.1-181 #r3071  
- add the engine part of handling kernel params.

* Mon Feb 16 2009 wppan <wppan@redflag-linux.com> 0.1-181 #r3069  
- add set+grub_args()

* Mon Feb 16 2009 leojiang <leojiang@redflag-linux.com> 0.1-181 #r3068  
- set the hostname in the /etc/sysconfig/work.

* Fri Feb 13 2009 leojiang <leojiang@redflag-linux.com> 0.1-180 #r3053  
- add the set kdeglobals functions.

* Wed Feb 11 2009 wppan <wppan@redflag-linux.com> 0.1-179 #r3036  
- correct mkfs reiserfs 

* Thu Feb 05 2009 wppan <wppan@redflag-linux.com> 0.1-178 #r3019  
- BUG9842  change device name length from 10 to 16

* Thu Feb 05 2009 wppan <wppan@redflag-linux.com> 0.1-178 #r3017  
- correct copy dt7_conf.xml error

* Thu Jan 08 2009 leojiang <leojiang@redflag-linux.com> 0.1-177 #r2965  
- impl the engine part of set hostname.

* Tue Jan 06 2009 leojiang <leojiang@redflag-linux.com> 0.1-176 #r2954  
- umount all partitions before start installation.

* Thu Dec 25 2008 leojiang <leojiang@redflag-linux.com> 0.1-175 #r2855  
- comment copy series function.

* Thu Dec 25 2008 leojiang <leojiang@redflag-linux.com> 0.1-175 #r2850  
- impl do_set_xorg.

* Thu Dec 25 2008 leojiang <leojiang@redflag-linux.com> 0.1-175 #r2848  
- impl do_set_tz.

* Thu Dec 25 2008 leojiang <leojiang@redflag-linux.com> 0.1-175 #r2847  
- set the kl file.

* Thu Dec 25 2008 leojiang <leojiang@redflag-linux.com> 0.1-175 #r2846  
- change the function name of xorgconf.

* Wed Dec 24 2008 wppan <wppan@redflag-linux.com> 0.1-174 #r2837  
- add rhgb

* Wed Dec 24 2008 jianxing <jianxing@redflag-linux.com> 0.1-173 #r2828  
- fix lijiang\'s error: add return value

* Wed Dec 24 2008 jianxing <jianxing@redflag-linux.com> 0.1-172 #r2819  
- remove grub-install-guide from installed system

* Wed Dec 24 2008 wppan <wppan@redflag-linux.com> 0.1-171 #r2814  
- restore dt7 entry in grub.conf

* Tue Dec 23 2008 wppan <wppan@redflag-linux.com> 0.1-170 #r2789  
- move ln -s grub.conf to postscript.tmpl

* Tue Dec 23 2008 wppan <wppan@redflag-linux.com> 0.1-169 #r2785  
- ln -s /etc/grub.conf to /boot/grub/grub.conf

* Tue Dec 23 2008 wppan <wppan@redflag-linux.com> 0.1-169 #r2783  
- set label to sdaXX

* Mon Dec 22 2008 leojiang <leojiang@redflag-linux.com> 0.1-168 #r2770  
- change the name of function.

* Mon Dec 22 2008 wppan <wppan@redflag-linux.com> 0.1-167 #r2757  
- change RedFlag label to a 8 bit number

* Fri Dec 19 2008 leojiang <leojiang@redflag-linux.com> 0.1-166 #r2751  
- change the param of zoneinfo.

* Fri Dec 19 2008 wppan <wppan@redflag-linux.com> 0.1-165 #r2748  
- add a - into label

* Thu Dec 18 2008 wppan <wppan@redflag-linux.com> 0.1-164 #r2736  
- delete DT7 entry in grub.conf

* Thu Dec 18 2008 wppan <wppan@redflag-linux.com> 0.1-164 #r2735  
- change label RedFlag Linux to RedFlagXXXX

* Tue Dec 16 2008 wppan <wppan@redflag-linux.com> 0.1-163 #r2706  
- change Redflag into RedFlag

* Mon Dec 15 2008 leojiang <leojiang@redflag-linux.com> 0.1-162 #r2702  
- add the necessary include files into .h

* Mon Dec 15 2008 leojiang <leojiang@redflag-linux.com> 0.1-161 #r2698  
- add the class qualifier.

* Mon Dec 15 2008 wppan <wppan@redflag-linux.com> 0.1-160 #r2695  
- change splash into rhgb

* Fri Dec 12 2008 leojiang <leojiang@redflag-linux.com> 0.1-159 #r2683  
- add the do_* func and refactory the head file.

* Tue Dec 09 2008 kaikang <kaikang@redflag-linux.com> 0.1-158 #r2661  
- check autoCreateRegistryHelper return value

* Tue Dec 09 2008 leojiang <leojiang@redflag-linux.com> 0.1-157 #r2657  
- add new cmds.

* Mon Dec 08 2008 leojiang <leojiang@redflag-linux.com> 0.1-156 #r2653  
- reindent.

* Fri Dec 05 2008 leojiang <leojiang@redflag-linux.com> 0.1-155 #r2639  
- correct the rmpart function.

* Fri Dec 05 2008 leojiang <leojiang@redflag-linux.com> 0.1-155 #r2638  
- mv the copy functions.

* Thu Dec 04 2008 leojiang <leojiang@redflag-linux.com> 0.1-154 #r2630  
- add the missing else.

* Thu Dec 04 2008 leojiang <leojiang@redflag-linux.com> 0.1-154 #r2629  
- add a missing space in cmd_copy.

* Wed Dec 03 2008 leojiang <leojiang@redflag-linux.com> 0.1-153 #r2620  
- make the part of code relative to kb tz xdriver into functions.

* Wed Dec 03 2008 leojiang <leojiang@redflag-linux.com> 0.1-152 #r2619  
- reindent part of code to import the readability.

* Wed Dec 03 2008 leojiang <leojiang@redflag-linux.com> 0.1-151 #r2617  
- add the gen_grub_conf.h into spec.

* Fri Nov 28 2008 leojiang <leojiang@redflag-linux.com> 0.1-150 #r2570  
- correct the wrong include files moving.

* Thu Nov 27 2008 leojiang <leojiang@redflag-linux.com> 0.1-148 #r2563  
- move the namespace decl statement.

* Thu Nov 27 2008 leojiang <leojiang@redflag-linux.com> 0.1-148 #r2562  
- remove the joke code.

* Thu Nov 27 2008 leojiang <leojiang@redflag-linux.com> 0.1-148 #r2561  
- reformat the code style.

* Thu Nov 27 2008 wppan <wppan@redflag-linux.com> 0.1-148 #r2560  
- add a blank in cp command

* Thu Nov 27 2008 leojiang <leojiang@redflag-linux.com> 0.1-148 #r2559  
- refactor the file structure.

* Thu Nov 27 2008 wppan <wppan@redflag-linux.com> 0.1-148 #r2558  
- correct config file 

* Thu Nov 27 2008 leojiang <leojiang@redflag-linux.com> 0.1-147 #r2552  
- comment the old code for testing.

* Thu Nov 27 2008 wppan <wppan@redflag-linux.com> 0.1-146 #r2546  
- add check functions

* Fri Nov 14 2008 wppan <wppan@redflag-linux.com> 0.1-145 #r2485  
- correct grub-install

* Fri Nov 14 2008 wppan <wppan@redflag-linux.com> 0.1-145 #r2483  
- indent postscript()

* Fri Nov 14 2008 wppan <wppan@redflag-linux.com> 0.1-144 #r2481  
- BUG 9248

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-143 #r2478  
- BUG 9248

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-143 #r2477  
- BUG 9248

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-143 #r2476  
- BUG 9248

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-143 #r2475  
- BUG 9248

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-143 #r2474  
- BUG 9248

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-143 #r2473  
- BUG 9248

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-143 #r2472  
- BUG 9248

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-143 #r2471  
- BUG 9248

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-143 #r2470  
- modify gen_grub_conf, support boot partition

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-142 #r2469  
- delete obsolete MACRO

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-142 #r2468  
- correct OSIdentify

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-142 #r2467  
- correct OSIdentify

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-142 #r2466  
- correct OSIdentify, delete warning 

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-142 #r2465  
- correct OSIdentify 

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-142 #r2464  
- correct OSIdentify 

* Thu Nov 13 2008 wppan <wppan@redflag-linux.com> 0.1-142 #r2463  
- correct OSIdentify 

* Tue Nov 11 2008 wppan <wppan@redflag-linux.com> 0.1-141 #r2439  
- BUG 9248 correct find os in boot partition

* Tue Nov 11 2008 wppan <wppan@redflag-linux.com> 0.1-141 #r2437  
- BUG 9248 correct find os in boot partition

* Tue Nov 11 2008 wppan <wppan@redflag-linux.com> 0.1-141 #r2436  
- BUG 9248 correct find os in boot partition

* Tue Nov 11 2008 wppan <wppan@redflag-linux.com> 0.1-141 #r2434  
- BUG 9248 correct boot partition

* Mon Nov 10 2008 wppan <wppan@redflag-linux.com> 0.1-141 #r2433  
- BUG 9248 code with boot partition when calls grub-install

* Fri Oct 31 2008 wppan <wppan@redflag-linux.com> 0.1-140 #r2357  
- correct os_identify 

* Fri Oct 31 2008 wppan <wppan@redflag-linux.com> 0.1-140 #r2354  
- osidentify mount error

* Fri Oct 31 2008 wppan <wppan@redflag-linux.com> 0.1-140 #r2353  
- change unlink to rmdir

* Fri Oct 31 2008 wppan <wppan@redflag-linux.com> 0.1-140 #r2351  
- correct ret value in postscript

* Thu Oct 30 2008 wppan <wppan@redflag-linux.com> 0.1-139 #r2348  
- BUG 9193 correct fstab

* Thu Oct 30 2008 wppan <wppan@redflag-linux.com> 0.1-138 #r2344  
- umount tmp_mountpoint

* Thu Oct 30 2008 wppan <wppan@redflag-linux.com> 0.1-137 #r2341  
- forget to mkdir /tmp/tmp_mountpoint

* Fri Oct 24 2008 wppan <wppan@redflag-linux.com> 0.1-136 #r2306  
- don't need to copy it to /etc/skel/

* Fri Oct 24 2008 wppan <wppan@redflag-linux.com> 0.1-135 #r2304  
- engine doesn't need to set Use=true in kxkbrc

* Thu Oct 23 2008 wppan <wppan@redflag-linux.com> 0.1-135 #r2302  
- set Use=true in kxkbrc

* Thu Oct 23 2008 wppan <wppan@redflag-linux.com> 0.1-135 #r2301  
- set Use=true in kxkbrc

* Thu Oct 23 2008 wppan <wppan@redflag-linux.com> 0.1-135 #r2300  
- copy kxkbrx ktimezonedrc to /etc/skel/

* Thu Oct 23 2008 wppan <wppan@redflag-linux.com> 0.1-135 #r2299  
- copy kxkbrx ktimezonedrc to /etc/skel/

* Thu Oct 23 2008 wppan <wppan@redflag-linux.com> 0.1-135 #r2298  
- change kxkbrc ktimezonedrc

* Thu Oct 23 2008 wppan <wppan@redflag-linux.com> 0.1-135 #r2297  
- change kdmrc

* Thu Oct 23 2008 wppan <wppan@redflag-linux.com> 0.1-134 #r2287  
- not mount swap

* Thu Oct 23 2008 wppan <wppan@redflag-linux.com> 0.1-133 #r2285  
- take care of swap when mount it

* Wed Oct 22 2008 wppan <wppan@redflag-linux.com> 0.1-132 #r2278  
- rpm -e firstconfig package

* Wed Oct 22 2008 wppan <wppan@redflag-linux.com> 0.1-131 #r2271  
- delete postscript

* Wed Oct 22 2008 wppan <wppan@redflag-linux.com> 0.1-131 #r2269  
- delete postscript

* Wed Oct 22 2008 wppan <wppan@redflag-linux.com> 0.1-130 #r2268  
- change kxkbrc ktimezonedrc directory

* Wed Oct 22 2008 wppan <wppan@redflag-linux.com> 0.1-129 #r2261  
- correct kdmsts file

* Wed Oct 22 2008 wppan <wppan@redflag-linux.com> 0.1-128 #r2248  
- correct /bin/umount 

* Tue Oct 21 2008 wppan <wppan@redflag-linux.com> 0.1-127 #r2237  
- call umount before every do_mkfs

* Tue Oct 21 2008 wppan <wppan@redflag-linux.com> 0.1-126 #r2229  
- not need copy i18n, delete it

* Mon Oct 20 2008 wppan <wppan@redflag-linux.com> 0.1-125 #r2206  
- copy kxkbrc ktimezonedrc i18n to proper dir

* Mon Oct 20 2008 wppan <wppan@redflag-linux.com> 0.1-124 #r2202  
- create kdmsts file in /var/lib/kdm/

* Fri Oct 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-122 #r2174  
- revert wrong commit 2173

* Fri Oct 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-121 #r2173  
- update all TARGETS TO PROGRAMS since cmake update to 2.6

* Mon Oct 13 2008 leojiang <leojiang@redflag-linux.com> 0.1-120 #r2122  
- add the parttable exist probe function.

* Mon Oct 13 2008 wppan <wppan@redflag-linux.com> 0.1-119 #r2118  
- delete postscript after installation

* Mon Oct 13 2008 wppan <wppan@redflag-linux.com> 0.1-119 #r2117  
- add luseradd to add chinese user name

* Mon Oct 06 2008 jianxing <jianxing@redflag-linux.com> 0.1-118 #r2087  
- update license

* Wed Sep 24 2008 wppan <wppan@redflag-linux.com> 0.1-117 #r2053  
- grub.conf default

* Mon Sep 22 2008 wppan <wppan@redflag-linux.com> 0.1-116 #r2042  
- change kdmrc for kdm

* Fri Sep 19 2008 leojiang <leojiang@redflag-linux.com> 0.1-115 #r2029  
- add a new function to count of primary partiiton include freespace.

* Wed Sep 17 2008 wppan <wppan@redflag-linux.com> 0.1-114 #r2017  
- fix splashimage char string bug

* Wed Sep 17 2008 wppan <wppan@redflag-linux.com> 0.1-113 #r2016  
- change gfxmenu

* Wed Sep 17 2008 leojiang <leojiang@redflag-linux.com> 0.1-112 #r2011  
- add the headline for partition list print.

* Tue Sep 16 2008 leojiang <leojiang@redflag-linux.com> 0.1-111 #r2007  
- modify the class constructor interface.

* Thu Sep 11 2008 leojiang <leojiang@redflag-linux.com> 0.1-110 #r1985  
- add two extended partition relative functions.

* Thu Sep 11 2008 jianxing <jianxing@redflag-linux.com> 0.1-110 #r1983  
- return false when read failed

* Wed Sep 10 2008 wppan <wppan@redflag-linux.com> 0.1-109 #r1969  
- add splashimage

* Thu Sep 04 2008 wppan <wppan@redflag-linux.com> 0.1-108 #r1907  
- grub: change silent to quiet

* Thu Sep 04 2008 wppan <wppan@redflag-linux.com> 0.1-107 #r1906  
- change swapfile dir

* Wed Sep 03 2008 wppan <wppan@redflag-linux.com> 0.1-106 #r1904  
- Feature 57 create swapfile for simple & anvanced mode

* Wed Sep 03 2008 wppan <wppan@redflag-linux.com> 0.1-105 #r1901  
- BUG8683 change fat32

* Wed Sep 03 2008 wppan <wppan@redflag-linux.com> 0.1-105 #r1900  
- BUG8639 change fat32

* Tue Sep 02 2008 leojiang <leojiang@redflag-linux.com> 0.1-104 #r1898  
- not mkfs extended partition when create the new partition.

* Tue Sep 02 2008 wppan <wppan@redflag-linux.com> 0.1-103 #r1894  
- add debug info

* Tue Sep 02 2008 wppan <wppan@redflag-linux.com> 0.1-102 #r1890  
- BUG8683 mkfs problem

* Fri Aug 22 2008 wppan <wppan@redflag-linux.com> 0.1-101 #r1835  
- bug-8683  change if-else style

* Fri Aug 22 2008 wppan <wppan@redflag-linux.com> 0.1-101 #r1833  
- bug-8683  change mkfs type

* Thu Aug 14 2008 wppan <wppan@redflag-linux.com> 0.1-100 #r1816  
- fat32--vfat  fat16--msdos

* Thu Aug 07 2008 leojiang <leojiang@redflag-linux.com> 0.1-99 #r1793  
- add the mkfs operation in do_mkpart_whole and do_mkpart_length.

* Wed Aug 06 2008 wppan <wppan@redflag-linux.com> 0.1-98 #r1786  
- deal with system id using sfdisk

* Tue Aug 05 2008 wppan <wppan@redflag-linux.com> 0.1-97 #r1783  
- add swapoff -a

* Tue Aug 05 2008 wppan <wppan@redflag-linux.com> 0.1-96 #r1781  
- mkswap

* Mon Aug 04 2008 leojiang <leojiang@redflag-linux.com> 0.1-95 #r1780  
- construct string form of partition, NOT compact any more, compare with M G T manually.

* Mon Aug 04 2008 wppan <wppan@redflag-linux.com> 0.1-95 #r1779  
- write mkfs commands into rfinstall.log

* Mon Aug 04 2008 jianxing <jianxing@redflag-linux.com> 0.1-94 #r1777  
- default use local header files, not header files in /usr/include

* Mon Aug 04 2008 wppan <wppan@redflag-linux.com> 0.1-93 #r1776  
- libparted++ create

* Mon Aug 04 2008 wppan <wppan@redflag-linux.com> 0.1-92 #r1775  
- correct swap 

* Mon Aug 04 2008 leojiang <leojiang@redflag-linux.com> 0.1-91 #r1771  
- change the parttable function name.

* Fri Aug 01 2008 leojiang <leojiang@redflag-linux.com> 0.1-90 #r1754  
- multiple the geom length with sector size.

* Fri Aug 01 2008 leojiang <leojiang@redflag-linux.com> 0.1-89 #r1750  
- comment littlepart code temporarily.

* Fri Aug 01 2008 jianxing <jianxing@redflag-linux.com> 0.1-88 #r1747  
- revert 1744

* Thu Jul 31 2008 haopeng <haopeng@redflag-linux.com> 0.1-87 #r1744  
- max progress is 99, and set to 100. installer GUI needs once 100% set

* Wed Jul 30 2008 wppan <wppan@redflag-linux.com> 0.1-86 #r1697  
- not use uuid for swap

* Wed Jul 30 2008 jianxing <jianxing@redflag-linux.com> 0.1-85 #r1677  
- #8534 remove rfinstaller in installed system

* Mon Jul 28 2008 wppan <wppan@redflag-linux.com> 0.1-84 #r1658  
- use sfdisk to change partition type

* Mon Jul 28 2008 wppan <wppan@redflag-linux.com> 0.1-83 #r1655  
- add tab in front of initrd

* Mon Jul 28 2008 wppan <wppan@redflag-linux.com> 0.1-83 #r1654  
- skip swap partition when generate grub.conf

* Mon Jul 28 2008 wppan <wppan@redflag-linux.com> 0.1-83 #r1653  
- delete () from os-type

* Mon Jul 28 2008 wppan <wppan@redflag-linux.com> 0.1-82 #r1651  
- correct grub.conf

* Mon Jul 28 2008 wppan <wppan@redflag-linux.com> 0.1-82 #r1650  
- modify loop every partition to find boot info

* Mon Jul 28 2008 wppan <wppan@redflag-linux.com> 0.1-82 #r1649  
- skip swap partition when generate grub.conf

* Mon Jul 28 2008 wppan <wppan@redflag-linux.com> 0.1-81 #r1648  
- mkswap & /etc/fstab 

* Fri Jul 25 2008 wppan <wppan@redflag-linux.com> 0.1-80 #r1645  
- add --no-floppy

* Fri Jul 25 2008 leojiang <leojiang@redflag-linux.com> 0.1-80 #r1644  
- filter those free part smaller than 10M.

* Thu Jul 24 2008 leojiang <leojiang@redflag-linux.com> 0.1-79 #r1633  
- add some comments.

* Wed Jul 23 2008 leojiang <leojiang@redflag-linux.com> 0.1-78 #r1624  
- change the isPrimary imple.

* Wed Jul 23 2008 leojiang <leojiang@redflag-linux.com> 0.1-77 #r1622  
- change the add part api: bool-->int in return value.

* Wed Jul 23 2008 leojiang <leojiang@redflag-linux.com> 0.1-76 #r1596  
- modify and add api for partlist.

* Tue Jul 22 2008 leojiang <leojiang@redflag-linux.com> 0.1-75 #r1595  
- add new api for find index of part.

* Tue Jul 22 2008 leojiang <leojiang@redflag-linux.com> 0.1-75 #r1593  
- change unit used by the add_by_whole into mega level.

* Tue Jul 22 2008 leojiang <leojiang@redflag-linux.com> 0.1-75 #r1592  
- set the part not to bigger than free part.

* Tue Jul 22 2008 kaikang <kaikang@redflag-linux.com> 0.1-75 #r1591  
- print the list after add part successful.

* Tue Jul 22 2008 leojiang <leojiang@redflag-linux.com> 0.1-74 #r1585  
- add debug info into add_by_whole.

* Tue Jul 22 2008 leojiang <leojiang@redflag-linux.com> 0.1-73 #r1584  
- add the debug info into. add some testcase.

* Tue Jul 22 2008 leojiang <leojiang@redflag-linux.com> 0.1-73 #r1583  
- commit before remove it.

* Thu Jul 17 2008 kaikang <kaikang@redflag-linux.com> 0.1-72 #r1553  
- search swap for /etc/fstab

* Thu Jul 17 2008 kaikang <kaikang@redflag-linux.com> 0.1-72 #r1552  
- filter Unknow fs type for os_indentify.cpp

* Wed Jul 16 2008 wppan <wppan@redflag-linux.com> 0.1-71 #r1551  
- add swap_find

* Tue Jul 15 2008 jhwu <jhwu@redflag-linux.com> 0.1-70 #r1538  
- add GPG-KEY support

* Mon Jul 14 2008 wppan <wppan@redflag-linux.com> 0.1-69 #r1526  
- correct dead loop 

* Mon Jul 14 2008 jhwu <jhwu@redflag-linux.com> 0.1-68 #r1523  
- cp xorg.conf to installed system

* Fri Jul 11 2008 jianxing <jianxing@redflag-linux.com> 0.1-67 #r1520  
- recompile

* Fri Jul 11 2008 wppan <wppan@redflag-linux.com> 0.1-66 #r1517  
- correct duplacate dt7 grub boot menu

* Fri Jul 11 2008 jianxing <jianxing@redflag-linux.com> 0.1-65 #r1516  
- mv useless pkgs to dir /old

* Fri Jul 11 2008 wppan <wppan@redflag-linux.com> 0.1-64 #r1515  
- modify kernel release using /proc/version

* Thu Jul 10 2008 jhwu <jhwu@redflag-linux.com> 0.1-63 #r1487  
- fix bug #8369: disk label

* Thu Jul 10 2008 jhwu <jhwu@redflag-linux.com> 0.1-62 #r1484  
- set default timezone to CST

* Tue Jul 08 2008 wppan <wppan@redflag-linux.com> 0.1-61 #r1477  
- change spalsh into splash

* Wed Jul 02 2008 wppan <wppan@redflag-linux.com> 0.1-60 #r1448  
- correct grub.conf initrd problem by pwp

* Tue Jul 01 2008 kaikang <kaikang@redflag-linux.com> 0.1-59 #r1429  
- bug fix for show the os name - bug 8262

* Mon Jun 30 2008 leojiang <leojiang@redflag-linux.com> 0.1-58 #r1421  
- fix the bug in remove blacklist functions.

* Thu Jun 26 2008 leojiang <leojiang@redflag-linux.com> 0.1-57 #r1414  
-  add the device blacklist. Not test at all. Need compiled, and yum install into liveCD to test.

* Thu Jun 26 2008 jianxing <jianxing@redflag-linux.com> 0.1-57 #r1410  
- add cmake script

* Wed Jun 25 2008 jianxing <jianxing@redflag-linux.com> 0.1-56 #r1393  
- fix buffer bug and mount fs bug

* Wed Jun 25 2008 kaikang <kaikang@redflag-linux.com> 0.1-56 #r1392  
- add delete

* Wed Jun 25 2008 kaikang <kaikang@redflag-linux.com> 0.1-56 #r1387  
- user umount command replace system call

* Tue Jun 24 2008 kaikang <kaikang@redflag-linux.com> 0.1-56 #r1384  
- little update

* Tue Jun 24 2008 jianxing <jianxing@redflag-linux.com> 0.1-55 #r1376  
- close finished process

* Tue Jun 24 2008 kaikang <kaikang@redflag-linux.com> 0.1-54 #r1371  
- use full path of /bin/mount

* Tue Jun 24 2008 jianxing <jianxing@redflag-linux.com> 0.1-53 #r1366  
- fix small bug

* Tue Jun 24 2008 jianxing <jianxing@redflag-linux.com> 0.1-52 #r1365  
- close live service

* Mon Jun 23 2008 jianxing <jianxing@redflag-linux.com> 0.1-51 #r1363  
- read img from device live-osimg-min, it is smaller than the img

* Sun Jun 22 2008 kaikang <kaikang@redflag-linux.com> 0.1-50 #r1359  
- use system("mount ") instead of mount() system call

* Thu Jun 19 2008 haopeng <haopeng@redflag-linux.com> 0.1-49 #r1338  
- add stdlib.h for atoi

* Thu Jun 19 2008 jianxing <jianxing@redflag-linux.com> 0.1-48 #r1336  
- gen initrd in postscript

* Thu Jun 19 2008 haopeng <haopeng@redflag-linux.com> 0.1-48 #r1334  
- use atoi index of calculating by hand

* Thu Jun 19 2008 jianxing <jianxing@redflag-linux.com> 0.1-48 #r1331  
- mk tmp root dir

* Thu Jun 19 2008 jianxing <jianxing@redflag-linux.com> 0.1-47 #r1328  
- revert codes

* Wed Jun 18 2008 jianxing <jianxing@redflag-linux.com> 0.1-46 #r1326  
- fix bug

* Wed Jun 18 2008 jianxing <jianxing@redflag-linux.com> 0.1-45 #r1324  
- add code for debug 

* Wed Jun 18 2008 jianxing <jianxing@redflag-linux.com> 0.1-44 #r1316  
- reorg workmode matrix

* Wed Jun 18 2008 jianxing <jianxing@redflag-linux.com> 0.1-43 #r1315  
- fix write configure file bug

* Wed Jun 18 2008 kaikang <kaikang@redflag-linux.com> 0.1-42 #r1314  
- add install of os_identify.h for spec 

* Wed Jun 18 2008 kaikang <kaikang@redflag-linux.com> 0.1-41 #r1313  
- add os_identify.h

* Wed Jun 18 2008 kaikang <kaikang@redflag-linux.com> 0.1-40 #r1312  
- don't check linux-swap before mount call

* Wed Jun 18 2008 jianxing <jianxing@redflag-linux.com> 0.1-39 #r1310  
- write log to installed disk

* Wed Jun 18 2008 kaikang <kaikang@redflag-linux.com> 0.1-38 #r1309  
- re-implement os_identify.cpp

* Wed Jun 18 2008 jianxing <jianxing@redflag-linux.com> 0.1-37 #r1308  
- many fix, now it can work

* Tue Jun 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-36 #r1302  
- add \n to output

* Tue Jun 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-36 #r1301  
- address error return

* Tue Jun 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-36 #r1300  
- mkdir for mount

* Tue Jun 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-36 #r1299  
- fix mount bug

* Tue Jun 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-36 #r1298  
- fix read/write bug

* Tue Jun 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-36 #r1295  
- small fix

* Tue Jun 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-36 #r1289  
- split cmds to install cmd and post cmd for install() and postscript()

* Tue Jun 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-36 #r1288  
- some small fix

* Tue Jun 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-36 #r1287  
- rewrite console mode, add some small fix

* Tue Jun 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-36 #r1286  
- always write config file

* Tue Jun 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-36 #r1285  
- tmp implement of set root passwd

* Tue Jun 17 2008 jianxing <jianxing@redflag-linux.com> 0.1-36 #r1284  
- use cmd to umount, not system call.

* Mon Jun 16 2008 leojiang <leojiang@redflag-linux.com> 0.1-35 #r1283  
- add the remove mapper function. Need test.

* Mon Jun 16 2008 wppan <wppan@redflag-linux.com> 0.1-35 #r1281  
- change squashfs_file dir

* Mon Jun 16 2008 wppan <wppan@redflag-linux.com> 0.1-34 #r1279  
- install base system v1

* Fri Jun 13 2008 haopeng <haopeng@redflag-linux.com> 0.1-33 #r1268  
- read /etc/yum_livecd.conf instead of default configure file for yum

* Fri Jun 13 2008 wppan <wppan@redflag-linux.com> 0.1-33 #r1267  
- change livecd to live

* Fri Jun 13 2008 wppan <wppan@redflag-linux.com> 0.1-32 #r1266  
- correct cmdAddPackage parameter from string to char

* Fri Jun 13 2008 wppan <wppan@redflag-linux.com> 0.1-31 #r1265  
- add install yumshell.h

* Fri Jun 13 2008 wppan <wppan@redflag-linux.com> 0.1-30 #r1264  
- change rfinstall_console main.cpp addpackage

* Fri Jun 13 2008 wppan <wppan@redflag-linux.com> 0.1-29 #r1262  
- change add package&group

* Fri Jun 13 2008 wppan <wppan@redflag-linux.com> 0.1-29 #r1261  
- copy base system use squashfs.img

* Fri Jun 13 2008 haopeng <haopeng@redflag-linux.com> 0.1-29 #r1260  
- add test

* Thu Jun 12 2008 haopeng <haopeng@redflag-linux.com> 0.1-29 #r1258  
- fix yumshell.h bug

* Thu Jun 12 2008 wppan <wppan@redflag-linux.com> 0.1-29 #r1256  
- correct ntfs error

* Thu Jun 12 2008 wppan <wppan@redflag-linux.com> 0.1-28 #r1250  
- add some codes

* Thu Jun 12 2008 wppan <wppan@redflag-linux.com> 0.1-27 #r1248  
- 2008-06-11

* Thu Jun 12 2008 haopeng <haopeng@redflag-linux.com> 0.1-27 #r1246  
- add rootdir parameter

* Wed Jun 11 2008 haopeng <haopeng@redflag-linux.com> 0.1-27 #r1243  
- not need to call sleep, use I/O block for spinning, just like event triger

* Wed Jun 11 2008 haopeng <haopeng@redflag-linux.com> 0.1-26 #r1237  
- update yumshell.h and yumshell.cpp, basic functions have completed, except the rootdir and localinstall

* Thu Jun 05 2008 jianxing <jianxing@redflag-linux.com> 0.1-25 #r1230  
- add yumshell files

* Thu Jun 05 2008 wppan <wppan@redflag-linux.com> 0.1-24 #r1225  
- fix fp buf since buf[LENGTH]

* Thu May 29 2008 leojiang <leojiang@redflag-linux.com> 0.1-23 #r1196  
- add the null terminator in do_rmpart.

* Thu May 29 2008 jianxing <jianxing@redflag-linux.com> 0.1-23 #r1192  
- initrd will come from kernel pkg, no reason to generate it here

* Tue May 27 2008 jianxing <jianxing@redflag-linux.com> 0.1-22 #r1185  
- fix bug: append err log to file

* Tue May 27 2008 jianxing <jianxing@redflag-linux.com> 0.1-21 #r1184  
- fix mount cmd bug

* Tue May 27 2008 jianxing <jianxing@redflag-linux.com> 0.1-20 #r1183  
- use cmd to mount since it will write mtab so that grub-install can work

* Tue May 27 2008 jianxing <jianxing@redflag-linux.com> 0.1-19 #r1182  
- move grub-install back to gen_grub

* Tue May 27 2008 jianxing <jianxing@redflag-linux.com> 0.1-19 #r1180  
- delete some useless files

* Tue May 27 2008 jianxing <jianxing@redflag-linux.com> 0.1-18 #r1179  
- fix postscript

* Tue May 27 2008 jianxing <jianxing@redflag-linux.com> 0.1-17 #r1176  
- fix script bugs

* Tue May 27 2008 jianxing <jianxing@redflag-linux.com> 0.1-17 #r1170  
- fix bugs in destructor and postscript

* Tue May 27 2008 jianxing <jianxing@redflag-linux.com> 0.1-17 #r1168  
- use postscript to do more works

* Mon May 26 2008 jianxing <jianxing@redflag-linux.com> 0.1-16 #r1165  
- add tmp fix, these steps should in postscripts

* Mon May 26 2008 jianxing <jianxing@redflag-linux.com> 0.1-15 #r1164  
- tmp fix of /home

* Mon May 26 2008 jianxing <jianxing@redflag-linux.com> 0.1-14 #r1163  
- small fix

* Mon May 26 2008 jianxing <jianxing@redflag-linux.com> 0.1-14 #r1162  
- smallfix

* Mon May 26 2008 jianxing <jianxing@redflag-linux.com> 0.1-14 #r1161  
- add user

* Mon May 26 2008 jianxing <jianxing@redflag-linux.com> 0.1-14 #r1159  
- fix grub file and generate initrd

* Mon May 26 2008 jianxing <jianxing@redflag-linux.com> 0.1-13 #r1150  
- fix fstab bug

* Mon May 26 2008 jianxing <jianxing@redflag-linux.com> 0.1-13 #r1149  
- fix grub bug

* Sun May 25 2008 jianxing <jianxing@redflag-linux.com> 0.1-12 #r1147  
- remove debug codes

* Sat May 24 2008 jianxing <jianxing@redflag-linux.com> 0.1-11 #r1137  
- fix spec files

* Fri May 23 2008 wppan <wppan@redflag-linux.com> 0.1-10 #r1135  
- modify getenv

* Fri May 23 2008 wppan <wppan@redflag-linux.com> 0.1-9 #r1130  
- add grub

* Fri May 23 2008 wppan <wppan@redflag-linux.com> 0.1-9 #r1127  
- add umount 

* Thu May 22 2008 jianxing <jianxing@redflag-linux.com> 0.1-8 #r1118  
- remove debug section

* Thu May 22 2008 jianxing <jianxing@redflag-linux.com> 0.1-8 #r1117  
- fix precent bug

* Thu May 22 2008 jianxing <jianxing@redflag-linux.com> 0.1-8 #r1116  
- rm rootfiles

* Thu May 22 2008 jianxing <jianxing@redflag-linux.com> 0.1-8 #r1115  
- pretty code

* Thu May 22 2008 jianxing <jianxing@redflag-linux.com> 0.1-8 #r1112  
- fix some crash bugs

* Thu May 22 2008 jianxing <jianxing@redflag-linux.com> 0.1-8 #r1110  
- add mklabel func

* Thu May 22 2008 jianxing <jianxing@redflag-linux.com> 0.1-8 #r1104  
- rename fix

* Wed May 21 2008 jianxing <jianxing@redflag-linux.com> 0.1-8 #r1102  
- rename rfinstall to rfinstaller

* Wed May 21 2008 wppan <wppan@redflag-linux.com> 0.1-7 #r1099  
- add progress

* Wed May 21 2008 wppan <wppan@redflag-linux.com> 0.1-6 #r1094  
- add calculate rpm number

* Wed May 21 2008 leojiang <leojiang@redflag-linux.com> 0.1-5 #r1086  
- add parted++ path into include directive

* Wed May 21 2008 wppan <wppan@redflag-linux.com> 0.1-5 #r1084  
- add copy_files

* Tue May 20 2008 wppan <wppan@redflag-linux.com> 0.1-4 #r1079  
- change (*progress) location

* Tue May 20 2008 wppan <wppan@redflag-linux.com> 0.1-4 #r1078  
- change (*progress) location

* Tue May 20 2008 wppan <wppan@redflag-linux.com> 0.1-4 #r1077  
- change (*progress) location

* Tue May 20 2008 wppan <wppan@redflag-linux.com> 0.1-4 #r1076  
- add (*progress)() for jiangli 

* Tue May 20 2008 leojiang <leojiang@redflag-linux.com> 0.1-4 #r1072  
- put the NULL at the right place 

* Tue May 20 2008 leojiang <leojiang@redflag-linux.com> 0.1-4 #r1071  
- add default param value NULL to engine.run() 

* Mon May 19 2008 jianxing <jianxing@redflag-linux.com> 0.1-3 #r1045  
- add some requires

* Mon May 19 2008 jianxing <jianxing@redflag-linux.com> 0.1-2 #r1044  
- fix required in spec

* Mon May 19 2008 jianxing <jianxing@redflag-linux.com> 0.1-2 #r1043  
- fix problems in spec and compiling

* Mon May 19 2008 jianxing <jianxing@redflag-linux.com> 0.1-2 #r1042  
- use cmake to make it.

* Mon May 19 2008 jianxing <jianxing@redflag-linux.com> 0.1-2 #r1041  
- reorgnize files, merge some small lib to engine lib

* Sun May 18 2008 leojiang <leojiang@redflag-linux.com> 0.1-2 #r1023  
- move grub findos partd++ engine to libinstallbase

