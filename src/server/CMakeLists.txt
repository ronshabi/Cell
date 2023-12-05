# -----------------------------------------------------------------------------
MESSAGE(STATUS "[CELL] Server Directory: " ${CMAKE_CURRENT_SOURCE_DIR})
# -----------------------------------------------------------------------------

add_executable(cellserver
        Main.cpp
        )

target_link_libraries(cellserver PRIVATE cell)