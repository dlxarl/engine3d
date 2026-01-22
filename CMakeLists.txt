cmake_minimum_required(VERSION 3.20)
project(3D_Engine VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW
FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# stb
FetchContent_Declare(stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
)
FetchContent_MakeAvailable(stb)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/glad.c

        src/Engine.cpp
        src/Input.cpp
        src/Shader.cpp
        src/Texture.cpp
        src/stb_image_impl.cpp   # <- ДОДАЙ

        src/Shape.cpp
        src/Cube.cpp
        src/Plane.cpp
        src/Sphere.cpp

        src/Skybox.cpp
        src/HUD.cpp
        src/PostProcessor.cpp
        src/Player.cpp

        # Scenes
        src/scenes/DemoScene.cpp
        src/scenes/DemoPhysics.cpp
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src/scenes/include
        ${glm_SOURCE_DIR}
        ${stb_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32 gdi32 user32 winmm shell32)
endif()
