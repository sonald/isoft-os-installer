#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")

set(SRCS os_identify.cpp installengine.cpp pkgmeta.cpp)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ../libparted++ 
    ${GLIB2_INCLUDE_DIRS} ${RPM_INCLUDE_DIRS})

if (LIBALPM_FOUND)
    message("libalpm found, link it")
    INCLUDE_DIRECTORIES(${LIBALPM_INCLUDE_DIRS})
    set(SRCS ${SRCS} alpminstaller.cpp ipacman.c)
endif (LIBALPM_FOUND)

ADD_LIBRARY(installengine SHARED ${SRCS})

TARGET_LINK_LIBRARIES(installengine ${LIBALPM_LIBRARIES})

INSTALL(TARGETS installengine DESTINATION lib)
INSTALL(FILES installengine.h DESTINATION include)
INSTALL(FILES os_identify.h DESTINATION include)
INSTALL(FILES alpminstaller.h DESTINATION include)
INSTALL(FILES pkgmeta.h DESTINATION include)
INSTALL(FILES ipacman.h DESTINATION include)
INSTALL(FILES postscript.tmpl DESTINATION share/apps/libinstallerbase)

