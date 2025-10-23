# The CMakeList --/MyEngine/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(MyEngine)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/GameEngineDeps/stb)

# Source files
set(SOURCES 
    src/main.cpp
    src/util/Texture.cpp
    src/util/stb_image.c
    src/systems/SpriteRenderer.cpp
    src/systems/Menu.cpp
    src/objects/Ball.cpp
    src/objects/Paddle.cpp
    src/objects/Brick.cpp
    src/systems/Game.cpp
    src/objects/Powerup.cpp
)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(MyEngine ${SOURCES})
target_link_libraries(MyEngine SDL2::SDL2 OpenGL::GL)