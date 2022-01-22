cmake_minimum_required(VERSION 3.8)

project(rapidfuzz CXX)
include(GNUInstallDirs)

option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)

set(HEADERS
    rapidfuzz/distance.hpp
	rapidfuzz/fuzz.hpp
	rapidfuzz/fuzz.impl
    rapidfuzz/rapidfuzz_all.hpp

	rapidfuzz/distance/Hamming.hpp
	rapidfuzz/distance/Hamming.impl
	rapidfuzz/distance/Indel.hpp
	rapidfuzz/distance/Indel.impl
	rapidfuzz/distance/Levenshtein.hpp
	rapidfuzz/distance/Levenshtein.impl

	rapidfuzz/details/common.hpp
	rapidfuzz/details/common_impl.hpp
	rapidfuzz/details/intrinsics.hpp
	rapidfuzz/details/matching_blocks.hpp
	rapidfuzz/details/SplittedSentenceView.hpp
	rapidfuzz/details/type_traits.hpp
	rapidfuzz/details/types.hpp
)

add_library(rapidfuzz INTERFACE)

# provide a namespaced alias for clients to 'link' against if RapidFuzz is included as a sub-project
add_library(rapidfuzz::rapidfuzz ALIAS rapidfuzz)

set_target_properties(rapidfuzz PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_compile_features(rapidfuzz INTERFACE cxx_std_14)

target_include_directories(rapidfuzz INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                                               "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")

install(TARGETS rapidfuzz
  EXPORT  rapidfuzz
)

install(DIRECTORY rapidfuzz
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

if(BUILD_BENCHMARKS)
  add_subdirectory(bench)
endif()
