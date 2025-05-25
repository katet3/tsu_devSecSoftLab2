cmake_minimum_required(VERSION 3.10)
project(SAT_DPLL)

set(CMAKE_CXX_STANDARD 14)

# Опция для выбора типа аллокации
option(USE_CUSTOM_ALLOCATOR "Использовать пользовательский аллокатор" OFF)

# Если используется пользовательский аллокатор, определяем макрос для компиляции
if(USE_CUSTOM_ALLOCATOR)
    add_definitions(-DUSE_CUSTOM_ALLOCATOR)
    message(STATUS "Компиляция с использованием пользовательского аллокатора")
else()
    message(STATUS "Компиляция с использованием стандартного аллокатора")
endif()
if(ENABLE_ASAN)
    message(STATUS "Включен Address Sanitizer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if(ENABLE_UBSAN)
    message(STATUS "Включен Undefined Behavior Sanitizer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()

if(ENABLE_TSAN)
    message(STATUS "Включен Thread Sanitizer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
endif()

if(ENABLE_MSAN)
    message(STATUS "Включен Memory Sanitizer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=memory")
endif()

# Общие исходные файлы для обоих исполняемых файлов
set(COMMON_SOURCES
    "BBV.cpp"
    "boolequation.cpp"
    "boolinterval.cpp"
    "Branching_strategy.cc"
    "./lib/Allocator/Allocator.cpp"
)

# Исходные файлы для основного приложения
set(MAIN_SOURCES
    ${COMMON_SOURCES}
    "main.cpp"
)

# Исходные файлы для стресс-теста аллокатора
set(STRESS_TEST_SOURCES
    "./lib/Allocator/Allocator.cpp"
    "allocator_test.cc"
)

add_executable(SAT_DPLL ${MAIN_SOURCES})

add_executable(allocator_test ${STRESS_TEST_SOURCES})

if(USE_CUSTOM_ALLOCATOR)
    set_target_properties(SAT_DPLL PROPERTIES OUTPUT_NAME "SAT_DPLL_custom")
endif()