cmake_minimum_required(VERSION 3.20)
project(MyEngine VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/glad.c
        src/Engine.cpp
        include/Engine.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
        include
        ${glm_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        ${CMAKE_DL_LIBS}
)