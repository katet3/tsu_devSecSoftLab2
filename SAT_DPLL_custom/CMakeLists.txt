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

# Пути для Windows
set(PVS_STUDIO_DIR "C:/Program Files (x86)/PVS-Studio")
set(PVS_STUDIO_ANALYZER "${PVS_STUDIO_DIR}/x64/PVS-Studio.exe")
set(PVS_STUDIO_PLOG "${PVS_STUDIO_DIR}/PlogConverter.exe")

# Проверка наличия PVS-Studio
if(EXISTS "${PVS_STUDIO_ANALYZER}")
    message(STATUS "PVS-Studio found: ${PVS_STUDIO_ANALYZER}")
    
    # Генерация compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    
    add_custom_target(pvs_analysis_simple
    COMMAND "${PVS_STUDIO_ANALYZER}" 
        analyze 
        --source-file "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" 
        --cl-params "main.cpp -std=c++11 -I${CMAKE_CURRENT_SOURCE_DIR}"
        --output pvs_report.plog
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
else()
    message(WARNING "PVS-Studio analyzer not found at ${PVS_STUDIO_ANALYZER}")
endif()
