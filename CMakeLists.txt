cmake_minimum_required(VERSION 3.31)
project(minesweeper)

# Collect source files
file(GLOB SOURCES src/*.cpp)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)

if (DEFINED EMSCRIPTEN)
    # Create executable for Emscripten target
    add_executable(minesweeper ${SOURCES})

    # Set compiler flags (optimization, side module, etc.)
    target_compile_options(minesweeper PRIVATE -Os -s SIDE_MODULE=1)

    # Set linker flags for Emscripten (bind, modularize, environment)
    target_link_options(minesweeper PRIVATE
        --bind
        "SHELL: -s MODULARIZE=1"
        "SHELL: -s ENVIRONMENT='web'"
        -lembind
        --emit-tsd ${PROJECT_NAME}.d.ts
    )


else()
    # Enable testing
    enable_testing()

    # Exclude main.cpp from the test build by creating a list that omits it
    file(GLOB TEST_SOURCES src/*.cpp)

    # Fetch Google Test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        main  # You can specify a particular tag, branch, or commit
    )
    FetchContent_MakeAvailable(googletest)

    # You can create either a STATIC or SHARED library
    add_library(minesweeper STATIC ${SOURCES})

    # Add Google Test for unit tests
    # Add the test executable without main.cpp
    add_executable(minesweeper_test ${TEST_SOURCES} test_cpp/minesweeper_test.cpp)
    
    # Add the src directory to the include path for the test executable
    target_include_directories(minesweeper_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(minesweeper_test minesweeper gtest gtest_main)
    # Add test
    add_test(NAME MinesweeperTest COMMAND minesweeper_test)

endif()
