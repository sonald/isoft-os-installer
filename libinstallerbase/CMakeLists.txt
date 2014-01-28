cmake_minimum_required(VERSION 2.6)

PROJECT( libinstallerbase)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

FIND_PACKAGE(PkgConfig REQUIRED)
pkg_search_module(GLIB2 REQUIRED glib-2.0)
# pkg_search_module(RPM REQUIRED rpm)
pkg_search_module(LIBALPM REQUIRED libalpm)

#SUBDIRS( engine libparted++ cetcosinstaller_console)
SUBDIRS( engine libparted++)
