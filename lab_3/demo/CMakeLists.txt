cmake_minimum_required(VERSION 3.10)
project(WindowsExploit)

# Настройки для MinGW 64-bit
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# Флаги компиляции (без -z execstack, так как MinGW его не поддерживает)
set(CMAKE_C_FLAGS "-fno-stack-protector -no-pie")

# Сборка EXE
add_executable(vuln_program vuln_program.c)
set_target_properties(vuln_program PROPERTIES SUFFIX ".exe")

# Для Windows DEP и так обычно выключен, поэтому -z execstack не нужен