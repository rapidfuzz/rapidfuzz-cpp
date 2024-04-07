# Cmake config largely taken from catch2
cmake_minimum_required(VERSION 3.5)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
  cmake_policy(SET CMP0135 NEW)
endif()

# detect if Catch is being bundled,
# disable testsuite in that case
if(NOT DEFINED PROJECT_NAME)
  set(NOT_SUBPROJECT ON)
  # If RapidFuzz is not being used as a subproject via `add_subdirectory`,
  # usually installation is required
  option(RAPIDFUZZ_INSTALL "Install rapidfuzz" ON)
else()
  set(NOT_SUBPROJECT OFF)
  # If RapidFuzz is being used as a subproject via `add_subdirectory`,
  # chances are that the "main project" does not include RapidFuzz headers
  # in any of its headers, in which case installation is not needed.
  option(RAPIDFUZZ_INSTALL "Install rapidfuzz (Projects embedding rapidfuzz may want to turn this OFF.)" OFF)
endif()

option(RAPIDFUZZ_BUILD_TESTING "Build tests" OFF)
option(RAPIDFUZZ_ENABLE_LINTERS "Enable Linters for the test builds" OFF)
option(RAPIDFUZZ_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(RAPIDFUZZ_BUILD_FUZZERS "Build fuzzers" OFF)

# RapidFuzz's build breaks if done in-tree. You probably should not build
# things in tree anyway, but we can allow projects that include RapidFuzz
# as a subproject to build in-tree as long as it is not in our tree.
if (CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()

project(rapidfuzz LANGUAGES CXX VERSION 3.0.4)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Basic paths
set(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCES_DIR ${BASE_DIR}/rapidfuzz)
set(TEST_DIR ${BASE_DIR}/test)
set(BENCHMARK_DIR ${BASE_DIR}/tests/bench)
set(EXAMPLES_DIR ${BASE_DIR}/examples)

add_library(rapidfuzz INTERFACE)

# provide a namespaced alias for clients to 'link' against if RapidFuzz is included as a sub-project
add_library(rapidfuzz::rapidfuzz ALIAS rapidfuzz)

target_compile_features(rapidfuzz INTERFACE cxx_std_17)

target_include_directories(rapidfuzz
    INTERFACE
      $<BUILD_INTERFACE:${SOURCES_DIR}/..>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Build tests only if requested
if(RAPIDFUZZ_BUILD_TESTING AND NOT_SUBPROJECT)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

# Build examples only if requested
if(RAPIDFUZZ_BUILD_EXAMPLES)
    #add_subdirectory(examples)
endif()

# Build benchmarks only if requested
if(RAPIDFUZZ_BUILD_BENCHMARKS)
  add_subdirectory(bench)
endif()

# Build fuzz tests only if requested
if(RAPIDFUZZ_BUILD_FUZZERS)
    add_subdirectory(fuzzing)
endif()

if (RAPIDFUZZ_INSTALL)
    set(RAPIDFUZZ_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/rapidfuzz")

    install(
        TARGETS
          rapidfuzz
        EXPORT
          rapidfuzzTargets
        DESTINATION
          ${CMAKE_INSTALL_LIBDIR}
    )

    install(
        EXPORT
          rapidfuzzTargets
        NAMESPACE
          rapidfuzz::
        DESTINATION
          ${RAPIDFUZZ_CMAKE_CONFIG_DESTINATION}
    )

    install(
        DIRECTORY
          rapidfuzz
        DESTINATION
          ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING
          PATTERN "*.hpp"
          PATTERN "*.impl"
    )

    configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${RAPIDFUZZ_CMAKE_CONFIG_DESTINATION}
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        COMPATIBILITY SameMajorVersion
    )

    install(
      FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
      DESTINATION
        ${RAPIDFUZZ_CMAKE_CONFIG_DESTINATION}
    )

    # CPack/CMake started taking the package version from project version 3.12
    # So we need to set the version manually for older CMake versions
    if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
        set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
    endif()

    set(CPACK_PACKAGE_VENDOR "Max Bachmann")
    set(CPACK_PACKAGE_CONTACT "https://github.com/rapidfuzz/rapidfuzz-cpp")
    include(CPack)

endif(RAPIDFUZZ_INSTALL)
