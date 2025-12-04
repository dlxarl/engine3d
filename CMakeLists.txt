cmake_minimum_required(VERSION 3.20)
project(3D_Engine VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.3.8)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(assimp GIT_REPOSITORY https://github.com/assimp/assimp.git GIT_TAG v5.4.3)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MINIZIP ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git GIT_TAG master)
FetchContent_MakeAvailable(stb)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/glad.c
        src/Engine.cpp
        include/Engine.h
        src/Input.cpp
        include/Input.h
        src/Shape.cpp
        include/Shape.h
        src/Cube.cpp
        include/Cube.h
        src/Shader.cpp
        include/Shader.h
        include/Scene.h
        src/scenes/DemoScene.cpp
        src/scenes/include/DemoScene.h
        src/Plane.cpp
        include/Plane.h
        src/Sphere.cpp
        include/Sphere.h
        src/Model.cpp
        include/Model.h
        ${assimp_SOURCE_DIR}/contrib/unzip/unzip.c
        ${assimp_SOURCE_DIR}/contrib/unzip/ioapi.c
        src/Texture.cpp
        include/Texture.h
        src/Skybox.cpp
        include/Skybox.h
        src/scenes/DemoPhysics.cpp
        src/scenes/include/DemoPhysics.h
        src/Player.cpp
        include/Player.h
        include/ShadowMap.h
        src/PostProcessor.cpp
        include/PostProcessor.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
        include
        ${glm_SOURCE_DIR}
        ${assimp_SOURCE_DIR}/include
        ${assimp_BINARY_DIR}/include
        ${assimp_SOURCE_DIR}/contrib/unzip
        ${stb_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        assimp
        z
        ${CMAKE_DL_LIBS}
)