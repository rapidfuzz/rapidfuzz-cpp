cmake_minimum_required(VERSION 3.14)
project(RapidFuzzUtilsHangul LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing() # Initialize CTest

# Find or Fetch Catch2
find_package(Catch2 3 QUIET)
if (Catch2_FOUND)
    message(STATUS "Using system-supplied version of Catch2")
else()
    message(STATUS "Using FetchContent to load Catch2")
    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
        GIT_TAG         v3.4.0
    )
    FetchContent_MakeAvailable(Catch2)
endif()

# Optional: Enable Linters if RAPIDFUZZ_ENABLE_LINTERS is ON
if (RAPIDFUZZ_ENABLE_LINTERS)
    message(STATUS "Enable Linters on test build")
    include(FetchContent)

    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.20)
        FetchContent_Declare(_project_options URL https://github.com/aminya/project_options/archive/refs/tags/v0.26.2.zip)
    else()
        FetchContent_Declare(_project_options URL https://github.com/aminya/project_options/archive/refs/tags/v0.25.2.zip)
    endif()
    FetchContent_MakeAvailable(_project_options)
    include(${_project_options_SOURCE_DIR}/Index.cmake)

    project_options(
            WARNINGS_AS_ERRORS
    )
endif()

# Exclude directories from globbing
set(EXCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/_deps" "${CMAKE_CURRENT_SOURCE_DIR}/build")

# Create a library for your utility functions
file(GLOB_RECURSE ALL_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

set(SOURCES "")
foreach(source_file ${ALL_SOURCES})
    # Exclude files in the _deps and build directories
    if (NOT source_file MATCHES "/_deps/" AND NOT source_file MATCHES "/build/")
        list(APPEND SOURCES "${source_file}")
    endif()
endforeach()

# For debugging: Print the list of source files (Added lines)
message(STATUS "List of source files:")
foreach(source_file ${SOURCES})
    message(STATUS "${source_file}")
endforeach()

add_library(RapidFuzzUtilsHangul STATIC ${SOURCES})

target_include_directories(RapidFuzzUtilsHangul PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(RapidFuzzUtilsHangul PUBLIC 
    ${CMAKE_BINARY_DIR}/_deps/catch2-build/generated-includes 
    ${CMAKE_BINARY_DIR}/_deps/catch2-src/src
)

# Function to add tests
function(utils_hangul_add_test test_file_path)
    get_filename_component(test_name ${test_file_path} NAME_WE) # Extract filename without extension
    string(REPLACE ".test" "" test_name ${test_name}) # Remove '.test' from the name

    add_executable(${test_name}.test ${test_file_path})
    target_link_libraries(${test_name}.test PRIVATE RapidFuzzUtilsHangul Catch2::Catch2WithMain)

    if (RAPIDFUZZ_ENABLE_LINTERS)
        target_link_libraries(${test_name}.test PRIVATE project_warnings)
    endif()
    add_test(NAME ${test_name} COMMAND ${test_name}.test)
endfunction()

# Discover all *.test.cpp files and add them as tests
message(STATUS "Discovering test files in ${CMAKE_CURRENT_SOURCE_DIR}")

# Exclude `build` and `_deps` directories when discovering test files
file(GLOB_RECURSE ALL_TEST_FILES
     "${CMAKE_CURRENT_SOURCE_DIR}/*.test.cpp"
)

set(TEST_FILES "")
foreach(test_file ${ALL_TEST_FILES})
    # Exclude test files in the _deps and build directories
    if (NOT test_file MATCHES "/_deps/" AND NOT test_file MATCHES "/build/")
        list(APPEND TEST_FILES "${test_file}")
    endif()
endforeach()

# Log the discovered test files for verification
foreach(test_file ${TEST_FILES})
    message(STATUS "Discovered test file: ${test_file}")
    utils_hangul_add_test(${test_file})
endforeach()
