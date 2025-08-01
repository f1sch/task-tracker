# src/CMakeLists.txt

# 1) TaskLib bauen
add_library(TaskLib
    Task.cpp
    TaskList.cpp
)

# 2) C++ standard
target_compile_features(TaskLib PUBLIC cxx_std_20)

# 3) Warn-Flags
target_link_libraries(TaskLib
    PRIVATE
        project_warnings
)

# 4) Include-Verzeichnisse sauber setzen
#    - BUILD_INTERFACE: für den Konsumenten während des Builds
#    - INSTALL_INTERFACE: wo die Headers nach 'make install' liegen
target_include_directories(TaskLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)
