cmake_minimum_required(VERSION 3.15...3.30)

project(pixelater)

set(CMAKE_CXX_FLAGS "-lraylib -lGL -lm -lpthread -ldl -lrt -lX11")

# target_include_directories(pixelater PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
# target_include_directories(pixelater PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/rlImGui)

file(GLOB SOURCE_FILES CONFIGURE_DEPENDS 
  "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/rlImGui/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/rlimgui/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/stb/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/assets/*"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/fonts/*")

# file(COPY "/assets" DESTINATION ${CMAKE_BINARY_DIR})
# file(COPY "/fonts" DESTINATION ${CMAKE_BINARY_DIR})
#

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_custom_target(copy_assets
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" ${CMAKE_CURRENT_BINARY_DIR}/assets
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/fonts" ${CMAKE_CURRENT_BINARY_DIR}/fonts
)

add_dependencies(${PROJECT_NAME} copy_assets)

# add_library(imGui imgui/imgui.cpp)
# target_link_libraries(pixelator PUBLIC imgui)


# add_library(rlImGui rlImGui/rlmImGui.cpp)
# target_link_libraries(pixelater PUBLIC rlImGui)
