cmake_minimum_required(VERSION 2.8)

project(rapidfuzz CXX)
include(GNUInstallDirs)

option(BUILD_TESTS "Build unit tests" OFF)

set(SOURCES
	rapidfuzz/fuzz.txx
	rapidfuzz/levenshtein.txx
	rapidfuzz/process.txx
	rapidfuzz/utils.txx
)

set(HEADERS
	rapidfuzz/fuzz.hpp
	rapidfuzz/levenshtein.hpp
	rapidfuzz/process.hpp
	rapidfuzz/utils.hpp
	rapidfuzz/details/SplittedSentenceView.hpp
	rapidfuzz/details/type_traits.hpp
	rapidfuzz/details/types.hpp
	rapidfuzz/details/unicode.hpp
	rapidfuzz/details/matching_blocks.hpp
	rapidfuzz/details/string_view.hpp
	rapidfuzz/details/optional.hpp
)

add_library(rapidfuzz INTERFACE)

# provide a namespaced alias for clients to 'link' against if RapidFuzz is included as a sub-project
add_library(rapidfuzz::rapidfuzz ALIAS rapidfuzz)

set_target_properties(rapidfuzz PROPERTIES PUBLIC_HEADER "${HEADERS}")

target_include_directories(rapidfuzz INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                                               "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")

install(TARGETS rapidfuzz
        EXPORT  rapidfuzz
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/rapidfuzz"
)

if(BUILD_TESTS)
  add_subdirectory(test)
endif()
