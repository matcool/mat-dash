cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mat-dash)
add_library(mat-dash INTERFACE)


include(cmake/CPM.cmake)

CPMAddPackage("gh:BepInEx/Dobby#1321565")

target_include_directories(mat-dash INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(mat-dash INTERFACE dobby)