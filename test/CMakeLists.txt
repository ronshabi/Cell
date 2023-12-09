# -----------------------------------------------------------------------------
MESSAGE(STATUS "[CELL] Test Directory: " ${CMAKE_CURRENT_SOURCE_DIR})
# -----------------------------------------------------------------------------
find_package(GTest REQUIRED)
include(GoogleTest)

add_compile_definitions(DEBUG)

enable_testing()

add_executable(StringTest StringTest.cpp)
target_link_libraries(StringTest PRIVATE GTest::gtest_main)
target_link_libraries(StringTest PRIVATE cell)
gtest_discover_tests(StringTest)
