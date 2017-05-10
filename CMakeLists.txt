cmake_minimum_required(VERSION 3.0)

project(libRCON C)

add_library(libRCON STATIC rcon.c rcon/rcon.h)
target_include_directories(libRCON PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(libRCON PROPERTIES C_STANDARD 90 OUTPUT_NAME "RCON")
if (UNIX OR APPLE)
	target_link_libraries(libRCON PUBLIC pthread)
endif ()


add_executable(RCON_example_client examples/rcon_client.c)
target_link_libraries(RCON_example_client PUBLIC libRCON)
set_target_properties(RCON_example_client PROPERTIES C_STANDARD 90)
