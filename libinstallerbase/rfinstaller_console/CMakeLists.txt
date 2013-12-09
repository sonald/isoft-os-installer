
# pkg_check_modules (GLIB2   glib-2.0)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ../engine ../libparted++
/usr/include/glib-2.0 /usr/lib/glib-2.0/include ${CMAKE_CURRENT_BINARY_DIR}
)

ADD_EXECUTABLE(rfinstaller_console main.cpp)

TARGET_LINK_LIBRARIES(rfinstaller_console installengine parted++ glib-2.0 ntreg parted)
INSTALL(TARGETS rfinstaller_console DESTINATION bin)

