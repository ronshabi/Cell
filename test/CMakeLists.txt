# -----------------------------------------------------------------------------
MESSAGE(STATUS "[CELL] Test Directory: " ${CMAKE_CURRENT_SOURCE_DIR})
# -----------------------------------------------------------------------------

set(Cell_Test_Sanitizers address,fuzzer,signed-integer-overflow,null,alignment,unsigned-integer-overflow)

find_package(GTest REQUIRED)
include(GoogleTest)

add_compile_definitions(DEBUG)

enable_testing()

add_executable(StringTest StringTest.cpp)
target_link_libraries(StringTest PRIVATE GTest::gtest_main)
target_link_libraries(StringTest PRIVATE cell)
gtest_discover_tests(StringTest)

add_executable(StringFuzz StringFuzz.cpp)
target_link_libraries(StringFuzz PRIVATE cell)
target_compile_options(StringFuzz PUBLIC -g -fsanitize=${Cell_Test_Sanitizers})
target_link_options(StringFuzz PUBLIC -g -fsanitize=${Cell_Test_Sanitizers})