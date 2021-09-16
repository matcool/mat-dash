cmake_minimum_required(VERSION 3.8)

add_library(mat-dash STATIC src/main.cpp)
add_subdirectory(libraries/minhook)
set_property(TARGET mat-dash PROPERTY CXX_STANDARD 17)
target_include_directories(mat-dash PUBLIC include)
target_link_libraries(mat-dash minhook)