Summary:	A common widget for installer & wintegrate
Name:		libdiskswidget
Distribution:	Red Flag Desktop 7.0
Packager:	Neil Kang
Vendor:		Red Flag
Version:	0.1
Release: 94
License:	GPL
Group:		Development/Libraries
Source:		%{name}-%{version}.tar.bz2
BuildRequires:	glib2, ntregedit, libinstallerbase
Requires:	glib2, ntregedit, libinstallerbase
BuildRoot:	/var/tmp/%{name}-%{version}-root
%description
A common widget used by Installer , wintegrate and grub restore

%prep
%setup 
#%setup -n %{name}

%build
lrelease-qt4 *.ts
qmake-qt4
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
* Fri Mar 27 2009 kaikang <kaikang@redflag-linux.com> 0.1-94 #r3359  
- I don't why, but international needs Q_OBJECT

* Mon Mar 23 2009 kaikang <kaikang@redflag-linux.com> 0.1-93 #r3313  
- remove vfat option

* Mon Mar 02 2009 kaikang <kaikang@redflag-linux.com> 0.1-92 #r3165  
- insert a space at the beginning of a string for alignment

* Fri Feb 20 2009 kaikang <kaikang@redflag-linux.com> 0.1-91 #r3108  
- expose osOnDevice() as new interface

* Mon Feb 16 2009 kaikang <kaikang@redflag-linux.com> 0.1-90 #r3072  
- a clerical error

* Mon Feb 16 2009 kaikang <kaikang@redflag-linux.com> 0.1-90 #r3070  
- if to be /, partition is forced to be formatted

* Fri Feb 13 2009 kaikang <kaikang@redflag-linux.com> 0.1-89 #r3058  
- bug9957 - unexpected reaction for double click

* Thu Feb 12 2009 kaikang <kaikang@redflag-linux.com> 0.1-88 #r3040  
- bug9290 - provide option to force to be primary partition

* Tue Jan 20 2009 kaikang <kaikang@redflag-linux.com> 0.1-87 #r2999  
- bug9205 - duplicated final partitions' info of simple installation

* Mon Jan 19 2009 kaikang <kaikang@redflag-linux.com> 0.1-86 #r2989  
- bug9659 - update translations

* Mon Jan 19 2009 kaikang <kaikang@redflag-linux.com> 0.1-86 #r2988  
- remove getchar()

* Mon Jan 19 2009 kaikang <kaikang@redflag-linux.com> 0.1-86 #r2986  
- bug9502: only can't show show windows os info

* Mon Jan 19 2009 kaikang <kaikang@redflag-linux.com> 0.1-86 #r2984  
- bug9469 - fix can not modify partition with vfat

* Mon Jan 19 2009 kaikang <kaikang@redflag-linux.com> 0.1-86 #r2983  
- respond to double click on item

* Tue Dec 30 2008 kaikang <kaikang@redflag-linux.com> 0.1-85 #r2932  
- add partition type for free partition when make part whole

* Tue Dec 30 2008 kaikang <kaikang@redflag-linux.com> 0.1-85 #r2931  
- add create fstable to command list

* Tue Dec 30 2008 kaikang <kaikang@redflag-linux.com> 0.1-85 #r2930  
- show create fstable dialog when click disk

* Fri Dec 26 2008 kaikang <kaikang@redflag-linux.com> 0.1-84 #r2898  
- update translation

* Wed Dec 03 2008 kaikang <kaikang@redflag-linux.com> 0.1-83 #r2614  
- for create fs table for new disk

* Wed Dec 03 2008 kaikang <kaikang@redflag-linux.com> 0.1-82 #r2608  
- I don't know why pro file has been deleted

* Tue Nov 11 2008 wppan <wppan@redflag-linux.com> 0.1-81 #r2439  
- BUG 9248 correct find os in boot partition

* Mon Nov 10 2008 kaikang <kaikang@redflag-linux.com> 0.1-80 #r2423  
- bug9224 -- update NEW button state when can not create Primary partition any more

* Tue Nov 04 2008 kaikang <kaikang@redflag-linux.com> 0.1-79 #r2389  
- bug9205: when install on a swap, fix not to show double messages

* Mon Nov 03 2008 kaikang <kaikang@redflag-linux.com> 0.1-78 #r2379  
- bug9202: can't install on the 16th partition 

* Mon Nov 03 2008 kaikang <kaikang@redflag-linux.com> 0.1-78 #r2365  
- rewrite DisksWidget::primaryCount using  interface provided by qtparted++

* Thu Oct 16 2008 kaikang <kaikang@redflag-linux.com> 0.1-77 #r2150  
- update translation for new line

* Mon Oct 13 2008 kaikang <kaikang@redflag-linux.com> 0.1-76 #r2123  
- revert last check in
- use Jiangli's new interface parttable.isExist()

* Mon Oct 13 2008 kaikang <kaikang@redflag-linux.com> 0.1-76 #r2119  
- store the disks pointer to avoid segment error

* Mon Sep 22 2008 kaikang <kaikang@redflag-linux.com> 0.1-75 #r2045  
- update translations

* Thu Sep 18 2008 kaikang <kaikang@redflag-linux.com> 0.1-74 #r2022  
- bug8785 - translations updated 

* Thu Sep 18 2008 kaikang <kaikang@redflag-linux.com> 0.1-73 #r2021  
- add create_fs_table(), when a device has no partition table, ask he to create

* Sun Sep 07 2008 kaikang <kaikang@redflag-linux.com> 0.1-72 #r1929  
- bug8633 - when click reset button, text on push button didn't be translated  

* Tue Sep 02 2008 kaikang <kaikang@redflag-linux.com> 0.1-71 #r1895  
- do not write to fstab when initiate the diskswidget

* Mon Aug 11 2008 kaikang <kaikang@redflag-linux.com> 0.1-70 #r1803  
- why I deleted the parted++ include path in pro file, confused!

* Mon Aug 11 2008 kaikang <kaikang@redflag-linux.com> 0.1-70 #r1802  
- set a upper limit to new partition size

* Mon Aug 04 2008 kaikang <kaikang@redflag-linux.com> 0.1-69 #r1778  
- add a comment

* Mon Aug 04 2008 kaikang <kaikang@redflag-linux.com> 0.1-68 #r1774  
- translation cases bug

* Mon Aug 04 2008 kaikang <kaikang@redflag-linux.com> 0.1-68 #r1773  
- edit a free space to create extended partition will make installation stop at 0%

* Mon Aug 04 2008 kaikang <kaikang@redflag-linux.com> 0.1-68 #r1772  
- confused! I have set conditions too strict

* Fri Aug 01 2008 kaikang <kaikang@redflag-linux.com> 0.1-67 #r1762  
- mkfs extended twice, installation will stay at 0%, fix it

* Fri Aug 01 2008 kaikang <kaikang@redflag-linux.com> 0.1-66 #r1761  
- add partition info with number, correct it using index

* Fri Aug 01 2008 kaikang <kaikang@redflag-linux.com> 0.1-65 #r1751  
- update ui files, add a main layout

* Thu Jul 31 2008 kaikang <kaikang@redflag-linux.com> 0.1-64 #r1725  
- update final partition info for advanced mode

* Thu Jul 31 2008 kaikang <kaikang@redflag-linux.com> 0.1-64 #r1724  
- fix a build bug

* Thu Jul 31 2008 kaikang <kaikang@redflag-linux.com> 0.1-63 #r1723  
- update doSimpleInstall(), use return value(num) from add_by_whole to search new create partition

* Thu Jul 31 2008 kaikang <kaikang@redflag-linux.com> 0.1-62 #r1720  
- update final partition information

* Thu Jul 31 2008 kaikang <kaikang@redflag-linux.com> 0.1-61 #r1713  
- add qm in source, update pro and spec file

* Thu Jul 31 2008 kaikang <kaikang@redflag-linux.com> 0.1-61 #r1712  
- rename ts file

* Wed Jul 30 2008 kaikang <kaikang@redflag-linux.com> 0.1-60 #r1704  
- spell error
- update ts file

* Wed Jul 30 2008 lfhan <lfhan@redflag-linux.com> 0.1-59 #r1700  
- add libdiskswidget.ts fill

* Wed Jul 30 2008 kaikang <kaikang@redflag-linux.com> 0.1-59 #r1698  
- mkfs vfat caused Unknown - bug 8547

* Wed Jul 30 2008 kaikang <kaikang@redflag-linux.com> 0.1-58 #r1687  
- it is so weird, assign a "" to QStrin parameter will fail initialization

* Wed Jul 30 2008 kaikang <kaikang@redflag-linux.com> 0.1-57 #r1675  
- add translator

* Wed Jul 30 2008 kaikang <kaikang@redflag-linux.com> 0.1-56 #r1674  
- update capacity check for /

* Wed Jul 30 2008 kaikang <kaikang@redflag-linux.com> 0.1-55 #r1670  
- spell error, bug 8532

* Mon Jul 28 2008 kaikang <kaikang@redflag-linux.com> 0.1-54 #r1660  
- update total and used size with proper unit 

* Mon Jul 28 2008 kaikang <kaikang@redflag-linux.com> 0.1-53 #r1657  
- re-write itemIndex()

* Mon Jul 28 2008 kaikang <kaikang@redflag-linux.com> 0.1-53 #r1656  
- add support for swap
- update start and end number of per-partition

* Thu Jul 24 2008 kaikang <kaikang@redflag-linux.com> 0.1-52 #r1637  
- do the right thing when get mount path from /etc/fstab

* Thu Jul 24 2008 kaikang <kaikang@redflag-linux.com> 0.1-51 #r1634  
- for get the right number of Used

* Thu Jul 24 2008 kaikang <kaikang@redflag-linux.com> 0.1-51 #r1632  
- ajust column width and sequence, but make used error, need to find out

* Thu Jul 24 2008 kaikang <kaikang@redflag-linux.com> 0.1-50 #r1631  
- when extended has no logical, lookup will error, correct it

* Wed Jul 23 2008 kaikang <kaikang@redflag-linux.com> 0.1-49 #r1627  
- update information of m_partInfoList whenever changes

* Wed Jul 23 2008 kaikang <kaikang@redflag-linux.com> 0.1-48 #r1626  
- code review

* Wed Jul 23 2008 kaikang <kaikang@redflag-linux.com> 0.1-47 #r1625  
- some inner variable name and code clean
- unmount when calculate used
- update to use Partition's isPrimary() to avoid show the trailing free in extended 

* Mon Jul 21 2008 kaikang <kaikang@redflag-linux.com> 0.1-46 #r1580  
- decrease the treewidget height for wintegrate 

* Mon Jul 21 2008 kaikang <kaikang@redflag-linux.com> 0.1-45 #r1576  
- update new button state

* Fri Jul 18 2008 kaikang <kaikang@redflag-linux.com> 0.1-44 #r1563  
- when delete a logical, device path will changed, result in bug 8461, fixed

* Thu Jul 17 2008 kaikang <kaikang@redflag-linux.com> 0.1-43 #r1556  
- modify fstab when try to mount every partition whose fs type is known

* Thu Jul 17 2008 kaikang <kaikang@redflag-linux.com> 0.1-42 #r1554  
- set partitions up limit 16

* Tue Jul 15 2008 kaikang <kaikang@redflag-linux.com> 0.1-41 #r1537  
- filter 'Unknown' for mount

* Tue Jul 15 2008 kaikang <kaikang@redflag-linux.com> 0.1-40 #r1533  
- when 'edit' /dev/sdb2, the result show at /dev/sda1, fix it

* Tue Jul 15 2008 kaikang <kaikang@redflag-linux.com> 0.1-39 #r1531  
- mount option -n is not valid for ntfs, so use system("umount") to erase the record in /etc/mtab

* Mon Jul 14 2008 kaikang <kaikang@redflag-linux.com> 0.1-38 #r1527  
- for mkfs twice in Advanced mode

* Mon Jul 14 2008 kaikang <kaikang@redflag-linux.com> 0.1-37 #r1524  
- expose new interface for simple mode
- fix mount parameter error

* Fri Jul 11 2008 kaikang <kaikang@redflag-linux.com> 0.1-36 #r1519  
- create partition, check the mount point - bug 8377

* Fri Jul 11 2008 kaikang <kaikang@redflag-linux.com> 0.1-35 #r1498  
- bug fix for 8387

* Fri Jul 11 2008 kaikang <kaikang@redflag-linux.com> 0.1-35 #r1497  
- bug fix for 8384

* Thu Jul 10 2008 kaikang <kaikang@redflag-linux.com> 0.1-34 #r1493  
- bug fix for can't mount ntfs partition - bug 8378

* Thu Jul 10 2008 kaikang <kaikang@redflag-linux.com> 0.1-33 #r1490  
- addpartition bug fix for 8374

* Tue Jul 08 2008 kaikang <kaikang@redflag-linux.com> 0.1-32 #r1475  
- fix bug 8326

* Tue Jul 08 2008 kaikang <kaikang@redflag-linux.com> 0.1-32 #r1474  
- adjust edit state cosistency, and change a button content

* Mon Jul 07 2008 kaikang <kaikang@redflag-linux.com> 0.1-31 #r1470  
- forget to do make_part_whole for simple installation

* Fri Jul 04 2008 kaikang <kaikang@redflag-linux.com> 0.1-30 #r1466  
- if disk doesn't has a partition table, omit it
- and provide a new interface

* Thu Jul 03 2008 kaikang <kaikang@redflag-linux.com> 0.1-29 #r1455  
- fix some compile warning

* Thu Jul 03 2008 kaikang <kaikang@redflag-linux.com> 0.1-28 #r1454  
- bug fix for editpartition

* Thu Jul 03 2008 kaikang <kaikang@redflag-linux.com> 0.1-28 #r1452  
- little ui change

* Thu Jul 03 2008 kaikang <kaikang@redflag-linux.com> 0.1-28 #r1451  
- simple installation adjust
- write profile bug fix - did one more mkfs wrongly

* Wed Jul 02 2008 kaikang <kaikang@redflag-linux.com> 0.1-27 #r1447  
- for find m_simpleInstallItem correctlly

* Wed Jul 02 2008 kaikang <kaikang@redflag-linux.com> 0.1-27 #r1446  
- for the warning info

* Wed Jul 02 2008 kaikang <kaikang@redflag-linux.com> 0.1-26 #r1444  
- for simple mode installation

* Tue Jul 01 2008 kaikang <kaikang@redflag-linux.com> 0.1-25 #r1433  
- set treewidget minimum height

* Tue Jul 01 2008 kaikang <kaikang@redflag-linux.com> 0.1-25 #r1432  
- add some info, update disks capacity warning

* Mon Jun 30 2008 jianxing <jianxing@redflag-linux.com> 0.1-24 #r1427  
- it is so weird. It is seems if there are more than ONE button in class definition willcause SEGMENT error. CONFUSING\!

* Mon Jun 30 2008 kaikang <kaikang@redflag-linux.com> 0.1-23 #r1426  
- add parent for 4 buttons

* Thu Jun 26 2008 kaikang <kaikang@redflag-linux.com> 0.1-22 #r1394  
- for simple mode segment error

* Wed Jun 25 2008 kaikang <kaikang@redflag-linux.com> 0.1-22 #r1390  
- set button new state for bug 7859

* Wed Jun 25 2008 kaikang <kaikang@redflag-linux.com> 0.1-22 #r1389  
- fix bug 7858

* Tue Jun 24 2008 kaikang <kaikang@redflag-linux.com> 0.1-22 #r1386  
- reason is same as previous one

* Tue Jun 24 2008 kaikang <kaikang@redflag-linux.com> 0.1-22 #r1385  
- In Windows mode, if top level item has no child, remove

* Wed Jun 18 2008 kaikang <kaikang@redflag-linux.com> 0.1-21 #r1305  
- change interface for os_identify

* Thu Jun 12 2008 kaikang <kaikang@redflag-linux.com> 0.1-20 #r1247  
- fix check '/' partition in edit and new widget

* Wed Jun 11 2008 kaikang <kaikang@redflag-linux.com> 0.1-20 #r1244  
- check "/" partition for capacity

* Fri Jun 06 2008 kaikang <kaikang@redflag-linux.com> 0.1-19 #r1233  
- check capacity for simple

* Fri Jun 06 2008 kaikang <kaikang@redflag-linux.com> 0.1-19 #r1232  
- check disk capacity for simple mode

* Tue Jun 03 2008 kaikang <kaikang@redflag-linux.com> 0.1-18 #r1211  
- for edit "free" partition

* Tue Jun 03 2008 kaikang <kaikang@redflag-linux.com> 0.1-18 #r1210  
- change new customize partition mininum to 100

* Tue Jun 03 2008 kaikang <kaikang@redflag-linux.com> 0.1-17 #r1200  
- set minimum size for directory tree

* Tue May 27 2008 kaikang <kaikang@redflag-linux.com> 0.1-16 #r1181  
- do make fs before set mount point

* Tue May 27 2008 kaikang <kaikang@redflag-linux.com> 0.1-15 #r1178  
- fix bug: make fs twice

* Tue May 27 2008 kaikang <kaikang@redflag-linux.com> 0.1-14 #r1175  
- fix bug : don't record the logic partition's ultimate status

* Tue May 27 2008 kaikang <kaikang@redflag-linux.com> 0.1-14 #r1169  
- do not view directory tree of swap partitions

* Fri May 23 2008 kaikang <kaikang@redflag-linux.com> 0.1-13 #r1134  
- fix some bugs with new edit

* Thu May 22 2008 jianxing <jianxing@redflag-linux.com> 0.1-12 #r1116  
- rm rootfiles

* Thu May 22 2008 kaikang <kaikang@redflag-linux.com> 0.1-12 #r1113  
- primary partition check

* Thu May 22 2008 kaikang <kaikang@redflag-linux.com> 0.1-12 #r1111  
- add and record make label operation
- fix the mess when extended before some primary

* Wed May 21 2008 kaikang <kaikang@redflag-linux.com> 0.1-11 #r1100  
- cancel delete partition warning
- make all line at uni-height

* Wed May 21 2008 kaikang <kaikang@redflag-linux.com> 0.1-10 #r1097  
- create fstab for a new disk

* Wed May 21 2008 kaikang <kaikang@redflag-linux.com> 0.1-9 #r1093  
- change include method of parted++

* Wed May 21 2008 kaikang <kaikang@redflag-linux.com> 0.1-8 #r1092  
- add new interface for wintegrate

* Tue May 20 2008 kaikang <kaikang@redflag-linux.com> 0.1-7 #r1069  
- fix use of itemIndex()

* Tue May 20 2008 kaikang <kaikang@redflag-linux.com> 0.1-6 #r1063  
- provide convenience for wintegrate

* Mon May 19 2008 kaikang <kaikang@redflag-linux.com> 0.1-5 #r1047  
- merge column.h to diskswidget.h

* Mon May 19 2008 jianxing <jianxing@redflag-linux.com> 0.1-4 #r1045  
- add some requires

* Mon May 19 2008 jianxing <jianxing@redflag-linux.com> 0.1-3 #r1043  
- fix problems in spec and compiling

* Mon May 19 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r1038  
- rename libdisklistwidget to libdiskswidget

* Mon May 19 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r1034  
- replace qmake with qmake-qt4 

* Thu May 15 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r1009  
- replace libxml with engine, use new interface

* Wed May 14 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r1003  
- modify for editpartiton() in diskswidget.cpp

* Wed May 14 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r1002  
- modify for addpartiton

* Wed May 14 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r1001  
- do not do rebuildtree in edit, and check exist mount point 
- if format is swap, disable mount point combobox

* Wed May 14 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r998  
- add error message for advanced

* Wed May 14 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r997  
- fix return root directory

* Wed May 14 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r996  
- get the size number replace -1 with -2

* Wed May 14 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r995  
- modify validate for simple mode

* Wed May 14 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r994  
- finish simple mode

* Wed May 14 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r990  
- update warningInfo & existMntPoint

* Wed May 14 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r989  
- fix emit sendPartInfo segment fault

* Tue May 13 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r988  
- fix don't iterate logical partition

* Mon May 12 2008 leojiang <leojiang@redflag-linux.com> 0.1-2 #r980  
- comment _mode != Linux 

* Mon May 12 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r979  
- add spec file and finish this project

* Mon May 12 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r973  
- add format operation

* Mon May 12 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r969  
- add .qrc file and images directory

* Mon May 12 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r968  
- add 3 more interface

* Sat May 10 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r964  
- add validate()

* Sat May 10 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r957  
- fix bug

* Sat May 10 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r956  
- modify addpartition.h to eliminate getTree()

* Sat May 10 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r955  
- fix a bug

* Sat May 10 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r954  
- add 3 interface

* Sat May 10 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r952  
- modify for emit sigLinuxInfo

* Sat May 10 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r951  
- modification for isPrimary

* Fri May 09 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r945  
- add target name

* Fri May 09 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r944  
- add column.h

* Fri May 09 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r942  
- for include path

* Fri May 09 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r941  
- ready for grub install use

* Fri May 09 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r933  
- finish record disks operations

* Thu May 08 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r923  
- half of write XML file

* Thu May 08 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r922  
- just for back up 

* Thu May 08 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r918  
- finish ui operations, ready to delete class TreeItem

* Wed May 07 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r903  
- add treeitem.h temporary, at last will delete

* Wed May 07 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r901  
- for penghao need a copy, so update

* Wed May 07 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r900  
- add pro file

* Tue May 06 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r895  
- meet a bug, can't hold information of logic partition

* Mon May 05 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r885  
- the end of May 5

* Mon May 05 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r879  
- finish delete and edit's most functionalities, before add partitions

* Tue Apr 29 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r850  
- finish delete a partition

* Mon Apr 28 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r846  
- add reset and delete, and delete seems not effect because of libparted++

* Mon Apr 28 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r845  
- same as last revision log

* Mon Apr 28 2008 kaikang <kaikang@redflag-linux.com> 0.1-2 #r844  
- init source, for elementary display
