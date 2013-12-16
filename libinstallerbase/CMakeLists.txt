cmake_minimum_required(VERSION 2.6)

PROJECT( libinstallerbase)

FIND_PACKAGE(PkgConfig REQUIRED)
pkg_search_module(GLIB2 REQUIRED glib-2.0)
pkg_search_module(RPM REQUIRED rpm)

#SUBDIRS( engine libparted++ cetcosinstaller_console)
SUBDIRS( engine libparted++)
