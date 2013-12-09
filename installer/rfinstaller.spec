Summary : Redflag Desktop 7.0 LiveCD Installer.
Name : rfinstaller
Version : 0.0.9
Release : 1
License : RedFlag License
URL : http://www.redflag-linux.com
Group : Applications/System
BuildRoot : /var/tmp/%{name}-%{version}-root
Requires : libinstallerbase libdiskswidget wintegrate qt
BuildRequires : libinstallerbase libdiskswidget wintegrate qt-devel
Source : %{name}-%{version}.tar.bz2

%description
Redflag Desktop 7.0 LiveCD Installer.

%prep
%setup -q

%build
qmake-qt4
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
make install INSTALL_ROOT=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_DIR/%{name}

%files
/usr/bin/rfinstaller
/usr/share/rfinstaller/translations/*
/usr/share/rfinstaller/images/*
/usr/share/applications/rfinstaller.desktop
/usr/lib/librfinsgame.so*
/usr/share/rfinstaller/svg/*
/etc/*.xml

%changelog
* Fri Oct 31 2008 wppan <wppan@redflag-linux.com> 0.9-88 #r2360  
- correct the partlimit of part automode.

* Thu Oct 23 2008 leojiang <leojiang@redflag-linux.com> 0.9-87 #r2290  
- add the reject dialog.

* Tue Oct 21 2008 leojiang <leojiang@redflag-linux.com> 0.9-86 #r2247  
- limit the username to [a-zA-Z0-9]-_. and CJK common.The char - can't be the first char.

* Tue Oct 21 2008 leojiang <leojiang@redflag-linux.com> 0.9-85 #r2243  
- remove the keyboardlayout, limit the passwd, ask for one user account.

* Tue Oct 21 2008 leojiang <leojiang@redflag-linux.com> 0.9-84 #r2224  
- correct the offset 1 error.

* Fri Oct 17 2008 leojiang <leojiang@redflag-linux.com> 0.9-83 #r2186  
- unique the application.

* Fri Oct 17 2008 leojiang <leojiang@redflag-linux.com> 0.9-82 #r2185  
- random the picture showed in progress page.

* Fri Oct 17 2008 leojiang <leojiang@redflag-linux.com> 0.9-81 #r2179  
- remove the Windows integration page.

* Fri Oct 17 2008 leojiang <leojiang@redflag-linux.com> 0.9-80 #r2176  
- enabled the windows integrate widget.

* Mon Oct 06 2008 jianxing <jianxing@redflag-linux.com> 0.9-79 #r2087  
- update license

* Sat Sep 27 2008 leojiang <leojiang@redflag-linux.com> 0.9-79 #r2085  
- set the restart computer as default.

* Sat Sep 27 2008 leojiang <leojiang@redflag-linux.com> 0.9-79 #r2083  
- add the checkbox into integrate windows page.

* Sat Sep 27 2008 leojiang <leojiang@redflag-linux.com> 0.9-79 #r2082  
- remove integration query.

* Sat Sep 27 2008 leojiang <leojiang@redflag-linux.com> 0.9-79 #r2079  
- correct the comment.

* Sat Sep 27 2008 leojiang <leojiang@redflag-linux.com> 0.9-79 #r2078  
- correct the condition about count limit of primary part.

* Sat Sep 27 2008 leojiang <leojiang@redflag-linux.com> 0.9-79 #r2077  
- trivial modification.

* Fri Sep 26 2008 leojiang <leojiang@redflag-linux.com> 0.9-79 #r2074  
- change the find code.

* Fri Sep 26 2008 leojiang <leojiang@redflag-linux.com> 0.9-78 #r2073  
- update the translate files.

* Fri Sep 26 2008 leojiang <leojiang@redflag-linux.com> 0.9-78 #r2071  
- open the automode.

* Fri Sep 26 2008 leojiang <leojiang@redflag-linux.com> 0.9-78 #r2069  
- add the automode into wizard.

* Thu Sep 25 2008 leojiang <leojiang@redflag-linux.com> 0.9-77 #r2066  
- add the defence code for absence of rflocale.xml.

* Thu Sep 25 2008 leojiang <leojiang@redflag-linux.com> 0.9-76 #r2065  
- open the URL link in external broswer.

* Thu Sep 25 2008 leojiang <leojiang@redflag-linux.com> 0.9-76 #r2064  
- remove grub page.

* Thu Sep 25 2008 leojiang <leojiang@redflag-linux.com> 0.9-75 #r2063  
- grub page is moved into summary, and automode is almost finished.

* Fri Sep 19 2008 leojiang <leojiang@redflag-linux.com> 0.9-74 #r2034  
- correct the include filename.

* Fri Sep 19 2008 leojiang <leojiang@redflag-linux.com> 0.9-74 #r2033  
- rename the files.

* Fri Sep 19 2008 leojiang <leojiang@redflag-linux.com> 0.9-74 #r2032  
- add the automode module.

* Tue Sep 16 2008 leojiang <leojiang@redflag-linux.com> 0.9-73 #r2005  
- change the path called relative to rflocale.xml

* Tue Sep 16 2008 leojiang <leojiang@redflag-linux.com> 0.9-73 #r2004  
- move the rflocale.xml into package redflag-release.

* Fri Sep 12 2008 leojiang <leojiang@redflag-linux.com> 0.9-72 #r1994  
- change the include line in mode.cpp

* Fri Sep 12 2008 leojiang <leojiang@redflag-linux.com> 0.9-72 #r1990  
- temporarily disable the auto mode of partition.

* Thu Sep 11 2008 leojiang <leojiang@redflag-linux.com> 0.9-71 #r1984  
- empty passwd is enabled, reduce the number of username.

* Tue Sep 09 2008 leojiang <leojiang@redflag-linux.com> 0.9-70 #r1957  
- use the parser_locale to deal the i18n. del the junk files.

* Tue Sep 09 2008 leojiang <leojiang@redflag-linux.com> 0.9-70 #r1951  
- add new file into profile. move locale.xml into translate dir.

* Tue Sep 09 2008 leojiang <leojiang@redflag-linux.com> 0.9-70 #r1950  
- add new class to impl i18n support.

* Fri Sep 05 2008 leojiang <leojiang@redflag-linux.com> 0.9-69 #r1919  
- move the groupeditor into directory tools.

* Thu Sep 04 2008 leojiang <leojiang@redflag-linux.com> 0.9-68 #r1908  
- change the name of files and class, adjust the pages of wizard.

* Mon Sep 01 2008 leojiang <leojiang@redflag-linux.com> 0.9-67 #r1878  
- remove rfgroups.xml from the installed files list.

* Mon Sep 01 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-66 #r1875  
- delete junk files.

* Mon Sep 01 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-66 #r1874  
- submit the groupeditorV2 for frozen coding.

* Sat Aug 16 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-65 #r1818  
- the version 2 of groupeditor

* Mon Aug 11 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-64 #r1796  
- basic complete

* Mon Aug 11 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-64 #r1795  
- modify little

* Mon Aug 11 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-63 #r1794  
- add groupeditor file in the svn

* Wed Aug 06 2008 jianxing <jianxing@redflag-linux.com> 0.9-62 #r1790  
- add header dir, fix compile error

* Wed Aug 06 2008 jianxing <jianxing@redflag-linux.com> 0.9-61 #r1788  
- change end time of license from 2007 to 2008

* Fri Aug 01 2008 leojiang <leojiang@redflag-linux.com> 0.9-60 #r1752  
- Not auto step to next page when progress end.

* Thu Jul 31 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-59 #r1743  
- correct the trans word in progress.

* Thu Jul 31 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-58 #r1727  
- update the translation.

* Thu Jul 31 2008 leojiang <leojiang@redflag-linux.com> 0.9-57 #r1726  
- add the new pic.

* Thu Jul 31 2008 leojiang <leojiang@redflag-linux.com> 0.9-56 #r1721  
- add the begin pic.

* Thu Jul 31 2008 leojiang <leojiang@redflag-linux.com> 0.9-56 #r1719  
- rename 030 to end.jpg

* Thu Jul 31 2008 leojiang <leojiang@redflag-linux.com> 0.9-56 #r1718  
- update pic-30.

* Thu Jul 31 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-56 #r1717  
- zhangqiang changed little of the .ts file

* Wed Jul 30 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-55 #r1709  
- zhangqiang commit the ts file

* Wed Jul 30 2008 leojiang <leojiang@redflag-linux.com> 0.9-54 #r1708  
- add the draft summary page.

* Wed Jul 30 2008 leojiang <leojiang@redflag-linux.com> 0.9-53 #r1703  
- modify text of simple page.

* Wed Jul 30 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-52 #r1702  
- update ts and qm.

* Tue Jul 29 2008 leojiang <leojiang@redflag-linux.com> 0.9-51 #r1666  
- modify some text, and update ts files.

* Tue Jul 29 2008 leojiang <leojiang@redflag-linux.com> 0.9-50 #r1665  
- modify the text.

* Tue Jul 29 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-49 #r1662  
- update zh_CN qm

* Tue Jul 29 2008 zhangqiang <zhangqiang@redflag-linux.com> 0.9-49 #r1661  
- update the ts.

* Mon Jul 28 2008 leojiang <leojiang@redflag-linux.com> 0.9-48 #r1659  
- change the interval of pictures, and add the transparent throbber.gif.

* Fri Jul 25 2008 leojiang <leojiang@redflag-linux.com> 0.9-47 #r1646  
- new pics.

* Fri Jul 25 2008 leojiang <leojiang@redflag-linux.com> 0.9-46 #r1643  
- add the missed pic 017.jpg and throbber in postscript dialog.

* Thu Jul 24 2008 leojiang <leojiang@redflag-linux.com> 0.9-45 #r1638  
- jump over the package selection step when run in LiveCD.

* Thu Jul 24 2008 leojiang <leojiang@redflag-linux.com> 0.9-44 #r1636  
- update the resource.

* Wed Jul 23 2008 leojiang <leojiang@redflag-linux.com> 0.9-43 #r1623  
- add the content into licence html files.

* Wed Jul 23 2008 leojiang <leojiang@redflag-linux.com> 0.9-42 #r1612  
- adjust code to new installer pics.

* Wed Jul 23 2008 leojiang <leojiang@redflag-linux.com> 0.9-41 #r1609  
- change the logo filename.

* Wed Jul 23 2008 leojiang <leojiang@redflag-linux.com> 0.9-41 #r1608  
- background.png

* Wed Jul 23 2008 leojiang <leojiang@redflag-linux.com> 0.9-41 #r1607  
- add the pics.

* Wed Jul 23 2008 leojiang <leojiang@redflag-linux.com> 0.9-40 #r1606  
- delete the junk pic files.

* Tue Jul 15 2008 leojiang <leojiang@redflag-linux.com> 0.9-39 #r1529  
- change the icons.

* Mon Jul 14 2008 leojiang <leojiang@redflag-linux.com> 0.9-38 #r1525  
- add the do real oparation before write conf. this functions just for simple, but null impl in three modes.

* Fri Jul 11 2008 jianxing <jianxing@redflag-linux.com> 0.9-37 #r1520  
- recompile

* Fri Jul 11 2008 leojiang <leojiang@redflag-linux.com> 0.9-37 #r1509  
- add the completeChanged SIGNAL in integration page.

* Thu Jul 10 2008 leojiang <leojiang@redflag-linux.com> 0.9-35 #r1496  
- adjust the order of page grub. handle the case of no Windows.

* Fri Jul 04 2008 leojiang <leojiang@redflag-linux.com> 0.9-34 #r1464  
- bug 7860 fixed, can be test by QA.

* Fri Jul 04 2008 leojiang <leojiang@redflag-linux.com> 0.9-33 #r1462  
- add the shortcut to buttons.

* Thu Jul 03 2008 leojiang <leojiang@redflag-linux.com> 0.9-32 #r1461  
- thread postscript. remove the cursor relative code to page.

* Thu Jul 03 2008 leojiang <leojiang@redflag-linux.com> 0.9-31 #r1459  
- use the thread to run postscript.

* Thu Jul 03 2008 leojiang <leojiang@redflag-linux.com> 0.9-30 #r1456  
- paint the dialog.

* Thu Jul 03 2008 leojiang <leojiang@redflag-linux.com> 0.9-29 #r1453  
- postscript dialog. no paint dialog yet.

* Tue Jul 01 2008 leojiang <leojiang@redflag-linux.com> 0.9-28 #r1431  
- add the dialog files.

* Tue Jul 01 2008 leojiang <leojiang@redflag-linux.com> 0.9-27 #r1430  
- add a dialog in finish page. the dialog will be show before finish page.

* Mon Jun 30 2008 leojiang <leojiang@redflag-linux.com> 0.9-26 #r1420  
- package page and postscript in finish page.

* Mon Jun 30 2008 leojiang <leojiang@redflag-linux.com> 0.9-26 #r1419  
- change the radiobutton position.

* Mon Jun 23 2008 leojiang <leojiang@redflag-linux.com> 0.9-25 #r1362  
- customize package selection is ok, and test.

* Sun Jun 22 2008 leojiang <leojiang@redflag-linux.com> 0.9-24 #r1357  
- add rfgroups.xml into files in spec.

* Fri Jun 20 2008 leojiang <leojiang@redflag-linux.com> 0.9-23 #r1352  
- add listwidget into package page. QMap is need to move out and sync with ListWidget.

* Fri Jun 20 2008 leojiang <leojiang@redflag-linux.com> 0.9-22 #r1350  
- add parser_custom*

* Fri Jun 20 2008 leojiang <leojiang@redflag-linux.com> 0.9-21 #r1341  
- add rfgroups.xml

* Fri Jun 20 2008 leojiang <leojiang@redflag-linux.com> 0.9-21 #r1340  
- add parsers

* Thu Jun 19 2008 jianxing <jianxing@redflag-linux.com> 0.9-20 #r1329  
- comment progress debuglog

* Wed Jun 18 2008 jianxing <jianxing@redflag-linux.com> 0.9-19 #r1317  
- mv desktop file to usr/share

* Wed Jun 18 2008 leojiang <leojiang@redflag-linux.com> 0.9-18 #r1304  
- add comps.xml. Check comps.xml into svn everytime comps.xml changed.

* Wed Jun 18 2008 leojiang <leojiang@redflag-linux.com> 0.9-18 #r1303  
- minimum mode package

* Tue Jun 17 2008 leojiang <leojiang@redflag-linux.com> 0.9-17 #r1296  
- misc modification.

* Tue Jun 17 2008 leojiang <leojiang@redflag-linux.com> 0.9-17 #r1292  
- add keyboard layout page, add the wintegrate widget.

* Fri Jun 13 2008 leojiang <leojiang@redflag-linux.com> 0.9-16 #r1263  
- add the fake addGroup statement

* Fri Jun 13 2008 jianxing <jianxing@redflag-linux.com> 0.9-16 #r1259  
- new category file, current from comps.xml

* Thu May 29 2008 leojiang <leojiang@redflag-linux.com> 0.9-15 #r1190  
- modify the text of grub page.

* Tue May 27 2008 leojiang <leojiang@redflag-linux.com> 0.9-14 #r1177  
- correct pro file.

* Tue May 27 2008 leojiang <leojiang@redflag-linux.com> 0.9-13 #r1174  
- remove connect type.

* Tue May 27 2008 leojiang <leojiang@redflag-linux.com> 0.9-13 #r1173  
- modify spec file to use make install.

* Tue May 27 2008 leojiang <leojiang@redflag-linux.com> 0.9-13 #r1172  
- Now install is enabled when free part is selected. Add INSTALLS in pro file. correct the access modifier of run().

* Tue May 27 2008 jianxing <jianxing@redflag-linux.com> 0.9-13 #r1168  
- use postscript to do more works

* Mon May 26 2008 leojiang <leojiang@redflag-linux.com> 0.9-13 #r1167  
- change the echo mode of passwd lineedit. add the run after set the instruction of add user.

* Mon May 26 2008 leojiang <leojiang@redflag-linux.com> 0.9-12 #r1153  
- add chinese name and comment for desktop file

* Sun May 25 2008 leojiang <leojiang@redflag-linux.com> 0.9-11 #r1145  
- delete the g_pointer to engine before quit.

* Sun May 25 2008 leojiang <leojiang@redflag-linux.com> 0.9-11 #r1143  
- modify the summary warning dialog behavior.

* Sun May 25 2008 leojiang <leojiang@redflag-linux.com> 0.9-11 #r1142  
- rm redflag user in spec.

* Sat May 24 2008 jianxing <jianxing@redflag-linux.com> 0.9-10 #r1138  
- fix spec

* Sat May 24 2008 jianxing <jianxing@redflag-linux.com> 0.9-9 #r1137  
- fix spec files

* Fri May 23 2008 leojiang <leojiang@redflag-linux.com> 0.9-8 #r1129  
- add rfinstaller.desktop file into repository. modify the spec.

* Fri May 23 2008 leojiang <leojiang@redflag-linux.com> 0.9-8 #r1128  
- add the installer icon file

* Fri May 23 2008 leojiang <leojiang@redflag-linux.com> 0.9-8 #r1126  
- adjust the progress pic size.

* Thu May 22 2008 leojiang <leojiang@redflag-linux.com> 0.9-7 #r1122  
-  temporarily disable some option. Add the quake pic.

* Thu May 22 2008 leojiang <leojiang@redflag-linux.com> 0.9-7 #r1114  
- add thread in progress

* Thu May 22 2008 leojiang <leojiang@redflag-linux.com> 0.9-7 #r1109  
- add layout for integratewindows page. comment the integrate widget because it need the ui.h. 

* Thu May 22 2008 leojiang <leojiang@redflag-linux.com> 0.9-7 #r1106  
- move the static pointer and function into wizardpage_progress.

* Wed May 21 2008 leojiang <leojiang@redflag-linux.com> 0.9-6 #r1098  
- change the g_Variable and progress.

* Tue May 20 2008 leojiang <leojiang@redflag-linux.com> 0.9-5 #r1074  
- remove ccc_large.xpm. this pic was used by package list.

* Tue May 20 2008 leojiang <leojiang@redflag-linux.com> 0.9-5 #r1073  
- add callback

* Mon May 19 2008 leojiang <leojiang@redflag-linux.com> 0.9-4 #r1052  
- correct the include file name

* Mon May 19 2008 leojiang <leojiang@redflag-linux.com> 0.9-4 #r1051  
- remove some -lxxx 

* Mon May 19 2008 leojiang <leojiang@redflag-linux.com> 0.9-3 #r1050  
- change the installer dir name

* Mon May 19 2008 leojiang <leojiang@redflag-linux.com> 2- #r1037  
- modify spec

* Mon May 19 2008 leojiang <leojiang@redflag-linux.com> 2- #r1036  
- remove -L path in LIBS

* Mon May 19 2008 leojiang <leojiang@redflag-linux.com> 2- #r1035  
- change InstallWizard to rfinstaller

* Mon May 19 2008 leojiang <leojiang@redflag-linux.com> 2- #r1033  
- change profile

* Sun May 18 2008 leojiang <leojiang@redflag-linux.com> 2- #r1030  
- create installer.spec

* Sun May 18 2008 leojiang <leojiang@redflag-linux.com> 2- #r1025  
- del dir InstallWizard

* Sun May 18 2008 leojiang <leojiang@redflag-linux.com> 2- #r1024  
- move InstallWizard/* ..

* Sun May 18 2008 leojiang <leojiang@redflag-linux.com> 2- #r1023  
- move grub findos partd++ engine to libinstallbase

* Sun May 18 2008 jianxing <jianxing@redflag-linux.com> 2- #r1020  
- update private funcs and add XXX for TODO

* Fri May 16 2008 leojiang <leojiang@redflag-linux.com> 2- #r1016  
- add wizardpage_partition and wizardpage_partition_disk page. modify summary grub

* Fri May 16 2008 jianxing <jianxing@redflag-linux.com> 2- #r1015  
- output cmd string

* Fri May 16 2008 jianxing <jianxing@redflag-linux.com> 2- #r1014  
- print args

* Fri May 16 2008 jianxing <jianxing@redflag-linux.com> 2- #r1013  
- fix constructor bug

* Thu May 15 2008 jianxing <jianxing@redflag-linux.com> 2- #r1010  
- update the instance and fix the resort bug

* Thu May 15 2008 jianxing <jianxing@redflag-linux.com> 2- #r1008  
- add prefix cmd to all set cmd func

* Thu May 15 2008 jianxing <jianxing@redflag-linux.com> 2- #r1007  
- if fail, return NULL, not exit

* Thu May 15 2008 jianxing <jianxing@redflag-linux.com> 2- #r1006  
- modify to singleton

* Thu May 15 2008 jianxing <jianxing@redflag-linux.com> 2- #r1005  
- rewrite engine, including functions of xmlconf

* Wed May 14 2008 leojiang <leojiang@redflag-linux.com> 2- #r999  
- partition widget ok.

* Wed May 14 2008 leojiang <leojiang@redflag-linux.com> 2- #r993  
- write do_part* code.

* Wed May 14 2008 jianxing <jianxing@redflag-linux.com> 2- #r992  
- Add makefile to engine and split it to lib and main

* Wed May 14 2008 jianxing <jianxing@redflag-linux.com> 2- #r991  
- add conf_tag and update to cpp

* Tue May 13 2008 wppan <wppan@redflag-linux.com> 2- #r987  
- add system(cp)

* Tue May 13 2008 wppan <wppan@redflag-linux.com> 2- #r986  
- add conf_clear()

* Mon May 12 2008 leojiang <leojiang@redflag-linux.com> 2- #r981  
- partition page framework working. before adjust the welcome initializePage.

* Sat May 10 2008 jianxing <jianxing@redflag-linux.com> 2- #r965  
- updaste spec file and remove some useless files

* Sat May 10 2008 leojiang <leojiang@redflag-linux.com> 2- #r961  
- useradd page

* Sat May 10 2008 leojiang <leojiang@redflag-linux.com> 2- #r960  
- complete useradd page. Find the doCommit scheme had logical bug, so use the validtePage to do next.

* Fri May 09 2008 leojiang <leojiang@redflag-linux.com> 2- #r950  
- add call function when press commit page.

* Fri May 09 2008 wppan <wppan@redflag-linux.com> 2- #r948  
- change interface for jiangli

* Fri May 09 2008 leojiang <leojiang@redflag-linux.com> 2- #r939  
- modify grub ui.

* Fri May 09 2008 leojiang <leojiang@redflag-linux.com> 2- #r937  
- add the page path into integration query.

* Fri May 09 2008 leojiang <leojiang@redflag-linux.com> 2- #r936  
- impl finish page.

* Fri May 09 2008 leojiang <leojiang@redflag-linux.com> 2- #r935  
- remove some junk files.

* Fri May 09 2008 leojiang <leojiang@redflag-linux.com> 2- #r934  
- remove the fake code of partition simple and advanced. add the textedit into summary.

* Thu May 08 2008 leojiang <leojiang@redflag-linux.com> 2- #r931  
- make the customize package redirect to summary temporarily.

* Thu May 08 2008 leojiang <leojiang@redflag-linux.com> 2- #r930  
- add rmall function.

* Thu May 08 2008 leojiang <leojiang@redflag-linux.com> 2- #r929  
- adjust conf file definition.

* Thu May 08 2008 kaikang <kaikang@redflag-linux.com> 2- #r927  
- eliminate "num", replace devpath with partpath -- Leo Jiang

* Thu May 08 2008 leojiang <leojiang@redflag-linux.com> 2- #r924  
- modify partition and package query page UI.

* Thu May 08 2008 leojiang <leojiang@redflag-linux.com> 2- #r921  
- modify installer sizeHint() to adjust the init size.

* Thu May 08 2008 leojiang <leojiang@redflag-linux.com> 2- #r920  
- modify the implementation of multi-locale support.

* Tue May 06 2008 leojiang <leojiang@redflag-linux.com> 2- #r898  
- move the localedescription structure into cpp file. change the sentence of root permission to be more friendly.

* Tue May 06 2008 leojiang <leojiang@redflag-linux.com> 2- #r894  
- remove Makefile and project file from svn repository.

* Tue May 06 2008 leojiang <leojiang@redflag-linux.com> 2- #r893  
- commit Makefile and project before rm them.

* Tue May 06 2008 leojiang <leojiang@redflag-linux.com> 2- #r892  
- remove select language page.

* Tue May 06 2008 leojiang <leojiang@redflag-linux.com> 2- #r891  
- change the licence filename and add the new file for pt.

* Tue May 06 2008 leojiang <leojiang@redflag-linux.com> 2- #r890  
- change the name of translation files.

* Tue May 06 2008 leojiang <leojiang@redflag-linux.com> 2- #r889  
- changed add_* functions name.

* Tue May 06 2008 wppan <wppan@redflag-linux.com> 2- #r888  
- my work done

* Mon May 05 2008 wppan <wppan@redflag-linux.com> 2- #r884  
-  system()==-1

* Mon May 05 2008 wppan <wppan@redflag-linux.com> 2- #r883  
- add extern C

* Mon May 05 2008 wppan <wppan@redflag-linux.com> 2- #r881  
- add engine.cpp

* Mon May 05 2008 leojiang <leojiang@redflag-linux.com> 2- #r880  
- add the parted-related functions.

* Mon May 05 2008 wppan <wppan@redflag-linux.com> 2- #r877  
- add engine.h

* Mon May 05 2008 wppan <wppan@redflag-linux.com> 2- #r876  
- modify PARTITION prefix

* Mon May 05 2008 wppan <wppan@redflag-linux.com> 2- #r872  
- add extern C

* Mon May 05 2008 wppan <wppan@redflag-linux.com> 2- #r871  
- add extern C 

* Sun May 04 2008 wppan <wppan@redflag-linux.com> 2- #r870  
- modify read_conf_section

* Sun May 04 2008 wppan <wppan@redflag-linux.com> 2- #r869  
- libxmlconf release 1

* Tue Apr 29 2008 wppan <wppan@redflag-linux.com> 2- #r853  
- libxml read/write ok

* Thu Apr 24 2008 wppan <wppan@redflag-linux.com> 2- #r827  
- add libxmlconf.so

* Thu Apr 24 2008 wppan <wppan@redflag-linux.com> 2- #r823  
- add libxmlconf

* Wed Apr 23 2008 wppan <wppan@redflag-linux.com> 2- #r820  
- add mnt_20080303_bak

* Wed Apr 23 2008 wppan <wppan@redflag-linux.com> 2- #r819  
- del mnt_20080303

* Wed Apr 23 2008 wppan <wppan@redflag-linux.com> 2- #r818  
- modify a little for freeing memory mnt_20080303_bak

* Wed Apr 23 2008 wppan <wppan@redflag-linux.com> 2- #r817  
- modify a little for freeing memory

* Wed Apr 23 2008 wppan <wppan@redflag-linux.com> 2- #r816  
- add free memory 

* Fri Apr 18 2008 wppan <wppan@redflag-linux.com> 2- #r789  
- modify Makefile. generate dynamic library

* Thu Apr 17 2008 wppan <wppan@redflag-linux.com> 2- #r778  
- add install grub

* Thu Apr 17 2008 wppan <wppan@redflag-linux.com> 2- #r777  
- 04-17

* Wed Apr 16 2008 wppan <wppan@redflag-linux.com> 2- #r772  
- libgrubconf  modify malloc

* Tue Apr 15 2008 wppan <wppan@redflag-linux.com> 2- #r764  
- libgrubconf  0.01

* Tue Apr 15 2008 wppan <wppan@redflag-linux.com> 2- #r763  
- libgrubconf 0.1  08-04-15

* Fri Apr 11 2008 leojiang <leojiang@redflag-linux.com> 2- #r757  
- rename wizardpage_selectlang.* to wizardpage_select_lang.*

* Fri Apr 11 2008 leojiang <leojiang@redflag-linux.com> 2- #r756  
- change the naming of part of source.

* Wed Apr 09 2008 wppan <wppan@redflag-linux.com> 2- #r746  
- add libgrub & libfindostpe files   pwp

* Wed Apr 09 2008 wppan <wppan@redflag-linux.com> 2- #r744  
- commit libgrub   pwp

* Wed Apr 09 2008 leojiang <leojiang@redflag-linux.com> 2- #r743  
- init installer and libs directories.
