

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

ADD_LIBRARY(parted++ SHARED  parttable.cpp partlist.cpp partition.cpp parted_devices.cpp devices.cpp device_unit.cpp)

INSTALL(TARGETS parted++ DESTINATION lib)
INSTALL(FILES device_unit.h DESTINATION include/parted++)
INSTALL(FILES parted++.h DESTINATION include/parted++)
INSTALL(FILES partition.h DESTINATION include/parted++)
INSTALL(FILES parttable.h DESTINATION include/parted++)
INSTALL(FILES devices.h DESTINATION include/parted++)
INSTALL(FILES parted_devices.h DESTINATION include/parted++)
INSTALL(FILES partlist.h DESTINATION include/parted++)
