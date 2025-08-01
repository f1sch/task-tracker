# GoogleTest einbinden
include(FetchContent)
include(GoogleTest)

# Try to find system-installed GoogleTest first
find_package(GTest QUIET)
if(GTest_FOUND)
    message(STATUS "Using system-installed GoogleTest")
else()
    message(STATUS "Fetching GoogleTest from GitHub")
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Test executables erstellen
add_executable(test_Task test_Task.cpp)
add_executable(test_TaskList test_TaskList.cpp)

# C++ Standard für Tests setzen
target_compile_features(test_Task PRIVATE cxx_std_20)
target_compile_features(test_TaskList PRIVATE cxx_std_20)

# Include directories für Tests
target_include_directories(test_Task PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(test_TaskList PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Libraries linken
if(GTest_FOUND)
    target_link_libraries(test_Task PRIVATE TaskLib GTest::gtest GTest::gtest_main)
    target_link_libraries(test_TaskList PRIVATE TaskLib GTest::gtest GTest::gtest_main)
else()
    target_link_libraries(test_Task PRIVATE TaskLib gtest_main)
    target_link_libraries(test_TaskList PRIVATE TaskLib gtest_main)
endif()

# Tests registrieren
gtest_discover_tests(test_Task)
gtest_discover_tests(test_TaskList)