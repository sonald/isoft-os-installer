cmake_minimum_required(VERSION 2.6)

PROJECT( libinstallerbase)

FIND_PACKAGE(PkgConfig REQUIRED)
pkg_search_module(GLIB2 REQUIRED glib-2.0)

#SUBDIRS( engine libparted++ rfinstaller_console)
SUBDIRS( engine libparted++)
