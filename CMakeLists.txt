cmake_minimum_required(VERSION 3.15...3.30)

project(
  PixelEditorCPP
  VERSION 0.1
  LANGUAGES CXX
)

add_library(imGui imgui/*.cpp)
target_link_libraries(imGui PUBLIC main.cpp)

add_library(rlImGui rlImGui/*.cpp)
target_link_libraries(rlImGui PUBLIC main.cpp)

add_executable(${PROJECT_NAME} main.cpp)
