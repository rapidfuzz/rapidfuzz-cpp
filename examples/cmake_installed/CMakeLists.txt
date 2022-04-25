cmake_minimum_required(VERSION 3.8)
project(cmake_installed CXX)

find_package(rapidfuzz REQUIRED)
add_executable(foo main.cpp)
target_link_libraries(foo rapidfuzz::rapidfuzz)