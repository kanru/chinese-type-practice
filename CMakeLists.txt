cmake_minimum_required(VERSION 2.6)
project(TypeWar)

include(FindPkgConfig)
pkg_check_modules(GTK2 REQUIRED gtk+-2.0>=2.10)

include_directories(${GTK2_INCLUDE_DIRS})

add_executable(tw src/tw.c)
target_link_libraries(tw ${GTK2_LIBRARIES})
