find_package(Catch2 3 REQUIRED)

function(rapidfuzz_add_test test)
    add_executable(test_${test} tests-${test}.cpp)
    target_link_libraries(test_${test} ${PROJECT_NAME})
    target_link_libraries(test_${test} Catch2::Catch2WithMain )
    add_test(NAME ${test} COMMAND test_${test})
endfunction()

rapidfuzz_add_test(fuzz)
rapidfuzz_add_test(common)

add_subdirectory(distance)
