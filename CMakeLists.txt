cmake_minimum_required(VERSION 3.08)
project(Count1)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-O2")

add_executable(Count1
        main.cpp
        map/map.cpp
)

set(SFML_DIR "C:/Users/Ruslan/CLionProjects/lib/sfml/GCC 7.3.0 MinGW (SEH) - 64-bit/SFML-2.5.1/lib/cmake/SFML")

target_link_libraries (Count1 sfml-graphics sfml-audio)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(SFML 2.5.1 REQUIRED graphics network audio window system)