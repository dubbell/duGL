cmake_minimum_required(VERSION 3.26)

project(duGL)

# Configure project policies.
include(scripts/config.cmake)

# Main executable target.
add_executable(${PROJECT_NAME} src/main.cpp)

# External dependencies.
include(FetchContent)
find_package(Python3 REQUIRED)
find_package(OpenGL REQUIRED)
include(scripts/GLFW.cmake)
include(scripts/GLAD.cmake)
include(scripts/GLM.cmake)
include(scripts/JoltPhysics.cmake)
include(scripts/assimp.cmake)
include(scripts/imgui.cmake)
include(scripts/stb.cmake)
#target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glad glfw glm::glm JoltPhysics assimp stb)

# Internal dependencies.
add_subdirectory(src)
target_link_libraries(${PROJECT_NAME} PUBLIC application controllers modelling shading view)

# Configure asset directories and variables.
include(scripts/assets.cmake)