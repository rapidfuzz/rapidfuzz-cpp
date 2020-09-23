cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(rapidfuzz CXX)
set(CMAKE_CXX_STANDARD 17)

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

add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(test)