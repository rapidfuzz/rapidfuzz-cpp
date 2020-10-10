cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(rapidfuzz CXX)
include(GNUInstallDirs)

option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_STATIC "Build static library" ON)
option(BUILD_SHARED "Build shared library" ON)

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

function(set_rapidfuzz_properties TARGET)
# provide a namespaced alias for clients to 'link' against if RapidFuzz is included as a sub-project
add_library(rapidfuzz::${TARGET} ALIAS ${TARGET})

set_target_properties(${TARGET} PROPERTIES VERSION 0.0.0
                                PUBLIC_HEADER ${HEADERS}
                                LINKER_LANGUAGE CXX
                                CXX_STANDARD 17)

target_include_directories(${TARGET} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
                                            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
                                            "${CMAKE_CURRENT_LIST_DIR}/extern")
install(TARGETS ${TARGET}
        EXPORT  ${TARGET}
        LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT shlib
        ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT lib
        RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"                            COMPONENT bin
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_TARGET_NAME}" COMPONENT dev
)
endfunction()

if(BUILD_SHARED)
  add_library(rapidfuzz SHARED ${SOURCES} ${HEADERS})
  set_rapidfuzz_properties(rapidfuzz)
endif()

if(BUILD_STATIC)
  add_library(rapidfuzz-static OBJECT ${SOURCES} ${HEADERS})
  set_rapidfuzz_properties(rapidfuzz-static)
endif()

if(BUILD_TESTS)
  add_subdirectory(test)
endif()