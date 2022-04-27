project(GraphDuplicates)
set(CMAKE_CXX_STANDARD 20)

# Find SFML shared libraries
find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

add_executable(GraphDuplicates src/main.cpp)

# Set include directory search paths
target_include_directories(GraphDuplicates 
  PRIVATE
    "${PROJECT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
  )

target_link_libraries(GraphDuplicates sfml-graphics sfml-audio sfml-window sfml-system)