CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

PROJECT(librcon C)

ADD_LIBRARY(librcon STATIC rcon.c rcon/rcon.h)
TARGET_INCLUDE_DIRECTORIES(librcon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE(rcon_example_client examples/rcon_client.c)
TARGET_LINK_LIBRARIES(rcon_example_client PUBLIC librcon)