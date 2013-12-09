
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ../libparted++ 
    ${GLIB2_INCLUDE_DIRS})

ADD_LIBRARY(installengine SHARED os_identify.cpp gen_grub_conf.cpp
installengine.cpp yumshell.cpp)

INSTALL(TARGETS installengine DESTINATION lib)
INSTALL(FILES installengine.h DESTINATION include)
INSTALL(FILES gen_grub_conf.h DESTINATION include)
INSTALL(FILES os_identify.h DESTINATION include)
INSTALL(FILES yumshell.h DESTINATION include)
INSTALL(FILES postscript.tmpl DESTINATION share/apps/libinstallerbase)

