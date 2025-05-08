cmake_minimum_required(VERSION 3.15)
project(SAT_DPLL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Явное перечисление исходников
set(SOURCES
    main.cpp
    ./lib/Allocator/Allocator.cpp
    BBV.cpp
    boolinterval.cpp
    boolequation.cpp
)

add_executable(sat_dpll ${SOURCES})
