# 1) Mindest-CMake-Version und Projektname
cmake_minimum_required(VERSION 3.10)
project(Task-Tracker VERSION 0.1 LANGUAGES CXX)

# Default auf Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build: Debug or Release" FORCE)
endif()


# 2) C++ Standard setzen
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 3) Include-Verzeichnis
include_directories(${PROJECT_SOURCE_DIR})

# 4) Quell Dateien
set(SOURCES
    main.cpp
    Task.cpp
    TaskList.cpp
)

# 5) Executable erzeugen
add_executable(task-cli ${SOURCES})

# 6) (Optional) Weitere Compiler-Flags
target_compile_options(task-cli PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# 7) (Optional) Install Regeln
install(TARGETS task-cli
    RUNTIME DESTINATION bin
)