find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(fuzzy_tests tests-main.cpp tests-levenshtein.cpp)
target_link_libraries(fuzzy_tests ${PROJECT_NAME})
target_link_libraries(fuzzy_tests Catch2::Catch2)
target_include_directories(fuzzy_tests PUBLIC ${CMAKE_SOURCE_DIR}/extern/)
target_include_directories(fuzzy_tests  PRIVATE ${Boost_INCLUDE_DIRS})
add_test(NAME tests COMMAND fuzzy_tests)
