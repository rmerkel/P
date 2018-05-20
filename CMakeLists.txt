################################################################################
# @file makefile
#
# @brief makefile for P language complier & interperter
################################################################################

cmake_minimum_required(VERSION 3.10)
project(P)
add_executable(p comp.cc compilier.cc datum.cc freestore.cc instr.cc interp.cc p.cc results.cc subrange.cc symbol.cc token.cc type.cc)
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -MMD -MP")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set_target_properties(p PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")
message("CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}")
message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
