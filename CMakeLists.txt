cmake_minimum_required (VERSION 2.8.12)
project (chitajson_test C)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall")
endif()

add_library(chitajson chitajson.c)
add_executable(chitajson_test test.c)
target_link_libraries(chitajson_test chitajson)
