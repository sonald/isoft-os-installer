set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ../libparted++ 
    ${GLIB2_INCLUDE_DIRS} ${RPM_INCLUDE_DIRS})

ADD_LIBRARY(installengine SHARED os_identify.cpp gen_grub_conf.cpp
installengine.cpp yumshell.cpp rpminstaller.cpp)

TARGET_LINK_LIBRARIES(installengine ${RPM_LIBRARIES})

INSTALL(TARGETS installengine DESTINATION lib)
INSTALL(FILES installengine.h DESTINATION include)
INSTALL(FILES gen_grub_conf.h DESTINATION include)
INSTALL(FILES os_identify.h DESTINATION include)
INSTALL(FILES yumshell.h DESTINATION include)
INSTALL(FILES postscript.tmpl DESTINATION share/apps/libinstallerbase)

