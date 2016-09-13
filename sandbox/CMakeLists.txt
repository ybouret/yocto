CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(hello)

INCLUDE(../share/yocto.cmake)

ADD_EXECUTABLE(hello hello.cpp)

