include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MyProject VERSION 1.0)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${GLAD_PATH}/src/glad.c)
set_source_files_properties(${GLAD_PATH}/src/glad.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(${GLAD_PATH}/src/glad.h PROPERTIES APPEND_STRING PROPERTY COMPILE_FLAGS " -Wnone")

add_executable(${PROJECT_NAME} ${SOURCES})

target_precompile_headers(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/pch.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${GLAD_PATH}/include
  ${GLFW_PATH}/include
  ${GLM_PATH}/include
  ${NETCDF_PATH}/include
  ${GENERAL_INCLUDES}
)

target_link_directories(${PROJECT_NAME} PUBLIC
  ${GLM_PATH}/bin
  ${GLM_PATH}/lib
  ${GLFW_PATH}/lib-vc2022
  ${NETCDF_PATH}/lib
  ${NETCDF_PATH}/bin
)

target_link_libraries(${PROJECT_NAME} glfw3 glm netcdf)
