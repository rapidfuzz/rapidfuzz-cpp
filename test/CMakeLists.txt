include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
        GIT_TAG         v3.0.0-preview4
)

FetchContent_MakeAvailable(Catch2)

function(rapidfuzz_add_test test)
    add_executable(test_${test} tests-${test}.cpp)
    target_link_libraries(test_${test} ${PROJECT_NAME})
    target_link_libraries(test_${test} Catch2::Catch2WithMain )
    add_test(NAME ${test} COMMAND test_${test})
endfunction()

rapidfuzz_add_test(fuzz)
#rapidfuzz_add_test(common)

add_subdirectory(distance)
