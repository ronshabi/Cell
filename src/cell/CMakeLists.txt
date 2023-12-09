# -----------------------------------------------------------------------------
MESSAGE(STATUS "[CELL] Library Directory: " ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "[CELL] Library Include Directory: " ${CMAKE_CURRENT_SOURCE_DIR}/../)
# -----------------------------------------------------------------------------

add_compile_definitions(DEBUG)

add_library(cell STATIC
        Assert.hpp
        Base.hpp
        Memory.hpp
        Charset.hpp
        String.cpp
        String.hpp
        Scanner.cpp
        Scanner.hpp)


target_include_directories(cell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
