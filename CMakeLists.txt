cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

################## LIBRARY ###################

project(c94m8p VERSION 0.0.1 DESCRIPTION "Driver of C94M8P")

add_library(c94m8p SHARED
    src/c94m8p.cpp
    src/utils/serial.cpp
    src/packet/packet.cpp
    src/packet/nmea_packet.cpp
    src/packet/ubx_packet.cpp
)

set_target_properties(c94m8p PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(c94m8p PROPERTIES PUBLIC_HEADER include/c94m8p.hpp)

target_include_directories(c94m8p PRIVATE include)

# Not ready for installation
#install(TARGETS c94m8p
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

################## DEFAULT ###################

project(default)

add_executable(default
    "test/default.cpp"
    )

target_link_libraries(default c94m8p)

# Temporary usage to link to the shared library
target_include_directories(default PRIVATE include)
target_link_directories(default PRIVATE build)


