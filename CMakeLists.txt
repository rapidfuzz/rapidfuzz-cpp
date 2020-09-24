cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(rapidfuzz CXX)
include(GNUInstallDirs)

set(SOURCES
	src/fuzz.txx
	src/levenshtein.txx
	src/process.txx
	src/utils.txx
	src/details/SentenceView.txx
)

set(HEADERS
	src/fuzz.hpp
	src/levenshtein.hpp
	src/process.hpp
	src/utils.hpp
	src/details/SentenceView.hpp
	src/details/SplittedSentenceView.hpp
	src/details/type_traits.hpp
	src/details/types.hpp
	src/details/unicode.hpp
)

add_library(rapidfuzz SHARED ${SOURCES} ${HEADERS})
set_target_properties(rapidfuzz PROPERTIES LINKER_LANGUAGE CXX)

option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
  add_subdirectory(test)
endif()

# provide a namespaced alias for clients to 'link' against if RapidFuzz is included as a sub-project
add_library(rapidfuzz::rapidfuzz ALIAS rapidfuzz)

set_target_properties(rapidfuzz PROPERTIES VERSION 0.0.0
                                PUBLIC_HEADER ${HEADERS}
                                CXX_STANDARD 17)

target_include_directories(rapidfuzz PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
                                            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
                                            "${CMAKE_CURRENT_LIST_DIR}/extern")
install(TARGETS rapidfuzz
        EXPORT  rapidfuzz
        LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT shlib
        ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT lib
        RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"                            COMPONENT bin
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_TARGET_NAME}" COMPONENT dev
)