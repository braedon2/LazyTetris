cmake_minimum_required(VERSION 3.14)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ( NOT CMAKE_BUILD_TYPE )
    message( "Build Type not set, defaulting to Debug..." )
    set( CMAKE_BUILD_TYPE Debug )
endif()

# Make a link to resources
file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    "${CMAKE_BINARY_DIR}/resources" SYMBOLIC)

# Set the project name. This is not the executable program's name!
project(lazy_tetris)

# Try to find a locally installed raylib, but don't quit on fail
find_package(raylib 5.5 QUIET)

# This code downloads raylib into a directory called _deps and adds it as a subdirectory, compiling it with the program when running the build command
include(FetchContent)
if (NOT raylib_FOUND)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP True #This option is not required but suppresses a warning
    )
    FetchContent_MakeAvailable(raylib)
endif()

# Fetch Google Test
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP True #This option is not required but suppresses a warning
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# We don't want raylib's examples built. This option is picked up by raylib's CMakeLists.txt
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Here, the executable is declared with its sources. "main", or "main.exe" on windows will be the program's name
include_directories(src)
add_executable(
  not_lazy 
  src/not_lazy.cpp 
  src/tetris.cpp 
  src/solver.cpp
)

# Link raylib to main
target_link_libraries(
    not_lazy 
    raylib
)
# Make main find the <raylib.h> header (and others)
target_include_directories(not_lazy PUBLIC "${raylib_SOURCE_DIR}/src")

add_executable(
  lazy_no_animation 
  src/lazy_no_animation.cpp 
  src/tetris.cpp 
  src/solver.cpp
)

# Link raylib to main
target_link_libraries(
    lazy_no_animation  
    raylib
)
# Make main find the <raylib.h> header (and others)
target_include_directories(lazy_no_animation  PUBLIC "${raylib_SOURCE_DIR}/src")

add_executable(
  lazy 
  src/lazy.cpp 
  src/tetris.cpp 
  src/solver.cpp
)

# Link raylib to main
target_link_libraries(
    lazy 
    raylib
)
# Make main find the <raylib.h> header (and others)
target_include_directories(lazy PUBLIC "${raylib_SOURCE_DIR}/src")

if (EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lidbfs.js -s USE_GLFW=3 --shell-file ${CMAKE_CURRENT_LIST_DIR}/web/minshell.html --preload-file ${CMAKE_CURRENT_LIST_DIR}/resources/@resources/ -s GL_ENABLE_GET_PROC_ADDRESS=1")
    set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
endif ()

# Set up testing
enable_testing()

add_executable(
  solver_test
  src/tetris.cpp 
  src/solver.cpp
  test/solver_test.cpp
)
target_link_libraries(
  solver_test
  GTest::gtest_main
  raylib
)

include(GoogleTest)
gtest_discover_tests(solver_test)