cmake_minimum_required(VERSION 3.8)
project(cmake_installed CXX)

find_package(rapidfuzz REQUIRED)
add_executable(foo main.cpp)

if(MSVC)
    target_compile_options(foo PRIVATE /W4 /WX)
else()
    target_compile_options(foo PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(foo rapidfuzz::rapidfuzz)