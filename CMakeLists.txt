# 1) Mindest-CMake-Version und Projektname
cmake_minimum_required(VERSION 3.16)
project(TaskTracker VERSION 0.2 LANGUAGES CXX)

# Default auf Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug or Release" FORCE)
endif()

# 2) C++ Standard wird in src/CMakeLists.txt mit target_compile_features gesetzt
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 3) (Optional) Weitere Compiler-Flags
add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)
# 4) Src-Unterverzeichnis einbinden, das TaskLib baut
add_subdirectory(src)

# 5) Tests einbinden (falls Tests aktiviert sind)
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 6) CLI-Executable definieren
add_executable(task-cli 
    main.cpp
)

# 7) C++ Standard für das Executable setzen
target_compile_features(task-cli PRIVATE cxx_std_20)

# 8) Linken der Bibliothek + Warnflags
target_include_directories(task-cli
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(task-cli PRIVATE TaskLib project_warnings)

# 9) (Optional) Install Regeln
install(TARGETS task-cli TaskLib project_warnings
    EXPORT TaskTrackerTargets
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib    
    LIBRARY DESTINATION lib    
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/ 
    DESTINATION include 
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

install(EXPORT TaskTrackerTargets
    FILE TaskTrackerTargets.cmake
    NAMESPACE TaskTracker::
    DESTINATION lib/cmake/TaskTracker
)