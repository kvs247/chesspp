cmake_minimum_required(VERSION 3.28)
project(chess VERSION 1.0 LANGUAGES CXX)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CONFIG_FILE_PATH "${CMAKE_SOURCE_DIR}/config/config.txt")
add_compile_definitions(CONFIG_FILE_PATH="${CONFIG_FILE_PATH}")

set(OUT_PGN_FILE_PATH "${CMAKE_SOURCE_DIR}/out/game.pgn")
add_compile_definitions(OUT_PGN_FILE_PATH="${OUT_PGN_FILE_PATH}")

# add compiler warnings
add_compile_options(-Wall -Wextra)

# define source files for chess executable
set(CHESS_SOURCES
  src/main.cpp
  src/piece.cpp
  src/game.cpp
  src/timeControl.cpp
  src/moveInput.cpp
  src/renderer/renderer.cpp
  src/renderer/frameBuilder.cpp
)

# define source files for tests
set(TEST_SOURCES
  tests/main.cpp
  tests/test_utils.cpp
  tests/test_pieces.cpp
  tests/test_game.cpp
  tests/test_render.cpp
  src/piece.cpp
  src/game.cpp
  src/timeControl.cpp
  src/moveInput.cpp
  src/renderer/renderer.cpp
  src/renderer/frameBuilder.cpp
)

# create chess executable
add_executable(chess ${CHESS_SOURCES})
target_include_directories(chess PRIVATE src)

# tests configuration
enable_testing()
find_package(GTest REQUIRED)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE GTest::GTest GTest::Main)

# add test to CTest
add_test(NAME unit_tests COMMAND tests)

# create custom targets
add_custom_target(run_tests
  COMMAND tests
  DEPENDS tests
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# set output directories
set_target_properties(chess tests
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
