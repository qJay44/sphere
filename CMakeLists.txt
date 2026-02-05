include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Run)

project(MyProject VERSION 1.0)

file(GLOB_RECURSE SOURCES
  ${PROJECT_SOURCE_DIR}/src/*.cpp
  ${UTILS_PATH}/src/*.cpp)

if (WIN32)
  set(LIB_EXT "dll")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(LIB_EXT "so")
# elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
#   set(LIB_EXT "dylib")
else()
  message(FATAL_ERROR "Unknown platform")
endif()

# Using mannually built imgui with GLFW (copying lib file)
set(GLFW_IMGUI_LIB_FILE libGLFW_IMGUI.${LIB_EXT})
add_custom_command(
  OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GLFW_IMGUI_LIB_FILE}
  COMMAND ${CMAKE_COMMAND} -E copy
    ${IMGUI_PATH}/build/${GLFW_IMGUI_LIB_FILE}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GLFW_IMGUI_LIB_FILE}
  DEPENDS ${IMGUI_PATH}/build/${GLFW_IMGUI_LIB_FILE}
)

# Using mannually built GLAD (copying lib file)
set(GLAD_LIB_FILE libGLAD.${LIB_EXT})
add_custom_command(
  OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GLAD_LIB_FILE}
  COMMAND ${CMAKE_COMMAND} -E copy
    ${GLAD_PATH}/build/${GLAD_LIB_FILE}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GLAD_LIB_FILE}
  DEPENDS ${GLAD_PATH}/build/${GLAD_LIB_FILE}
)

find_package(glfw3 REQUIRED)
find_package(TIFF REQUIRED)

add_executable(${PROJECT_NAME}
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GLFW_IMGUI_LIB_FILE}
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GLAD_LIB_FILE}
  ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
target_compile_options(${PROJECT_NAME} PRIVATE -isystem ${GENERAL_INCLUDES}/stb)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/pch.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${STB_PATH}/include
  ${GLM_PATH}/include
  ${GLAD_PATH}/include
  ${IMGUI_PATH}
  ${IMGUI_PATH}/backends
  ${TOL_PATH}/include
  ${UTILS_PATH}/include
)

target_link_directories(${PROJECT_NAME} PUBLIC
  ${GLM_PATH}/bin
  ${IMGUI_PATH}/build
  ${GLAD_PATH}/build
)

target_link_libraries(${PROJECT_NAME} GLFW_IMGUI GLAD TIFF::TIFF glfw)

