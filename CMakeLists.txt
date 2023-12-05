cmake_minimum_required(VERSION 3.25)
project(Cell)

add_compile_options(-fdiagnostics-color=always)
add_compile_options(-Wall -Wextra -Weffc++ -Wformat)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src/cell)
add_subdirectory(src/server)
add_subdirectory(test)