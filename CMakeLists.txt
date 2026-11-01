cmake_minimum_required(VERSION 3.20)
project(PacmanCpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(pacman
    src/main.cpp
    src/Game.cpp
    src/Maze.cpp
    src/Player.cpp
    src/Ghost.cpp
)

target_include_directories(pacman PRIVATE include)
target_link_libraries(pacman PRIVATE sfml-graphics sfml-window sfml-system)

if(MSVC)
    target_compile_options(pacman PRIVATE /W4 /permissive-)
else()
    target_compile_options(pacman PRIVATE -Wall -Wextra -Wpedantic)
endif()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
