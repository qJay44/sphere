include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Run)

project(MyProject VERSION 1.0)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${GLAD_PATH}/src/glad.c ${IMGUI_PATH}/*.cpp)
set_source_files_properties(${GLAD_PATH}/src/glad.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

add_executable(${PROJECT_NAME} ${SOURCES})

target_precompile_headers(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/pch.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${GLAD_PATH}/include
  ${GLFW_PATH}/include
  ${GLM_PATH}/include
  ${GENERAL_INCLUDES}
  ${IMGUI_PATH}
)

target_link_directories(${PROJECT_NAME} PUBLIC
  ${GLM_PATH}/bin
  ${GLFW_PATH}/lib-mingw-w64
  ${GLM_PATH}/lib
)

target_link_libraries(${PROJECT_NAME} glfw3 glm)
