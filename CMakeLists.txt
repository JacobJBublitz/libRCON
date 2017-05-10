cmake_minimum_required(VERSION 3.0)

project(libRCON C)

add_library(libRCON_static STATIC rcon.c rcon/rcon.h)
add_library(libRCON_shared SHARED rcon.c rcon/rcon.h)
target_include_directories(libRCON_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(libRCON_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(libRCON_static PROPERTIES C_STANDARD 90 OUTPUT_NAME "RCON")
set_target_properties(libRCON_shared PROPERTIES C_STANDARD 90 OUTPUT_NAME "RCON")
if (UNIX OR APPLE)
	target_link_libraries(libRCON_static PUBLIC pthread)
	target_link_libraries(libRCON_shared PUBLIC pthread)
endif ()


add_executable(RCON_example_client examples/rcon_client.c)
target_link_libraries(RCON_example_client PUBLIC libRCON_static)
set_target_properties(RCON_example_client PROPERTIES C_STANDARD 90)
