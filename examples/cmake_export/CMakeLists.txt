cmake_minimum_required(VERSION 3.5)
project(foo LANGUAGES CXX VERSION 0.0.1)

# The example library publicly dependent on RapidFuzz (includes
# rapidfuzz.hpp in foo_lib.hpp), necessitating RapidFuzz's installation
set(RAPIDFUZZ_INSTALL ON CACHE INTERNAL "")
add_subdirectory(${CMAKE_SOURCE_DIR}/../..
                 ${CMAKE_SOURCE_DIR}/../../build)

add_library(foo foo_lib.cc)
add_library(foo::foo ALIAS foo)
target_link_libraries(foo rapidfuzz)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(FOO_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/foo")
install(TARGETS foo EXPORT fooTargs DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(EXPORT fooTargs NAMESPACE foo:: DESTINATION ${FOO_CMAKE_CONFIG_DESTINATION})

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${FOO_CMAKE_CONFIG_DESTINATION}
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
    ${FOO_CMAKE_CONFIG_DESTINATION}
)
install(FILES foo_lib.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
