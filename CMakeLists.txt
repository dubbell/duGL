cmake_minimum_required(VERSION 3.20)

project(duGL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

add_compile_definitions(ASSIMP_STATIC)

add_library(project_dependencies INTERFACE)

target_include_directories(project_dependencies INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/include
)

add_subdirectory(src)

add_executable(${PROJECT_NAME} src/main.cpp lib/glad/src/gl.c)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC project_dependencies
    PRIVATE application
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/lib-vc2022/glfw3.lib
    opengl32.lib
    user32.lib
    gdi32.lib
    shell32.lib
    $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/lib/Debug/assimp-vc143-mtd.lib>
    $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/lib/Release/assimp-vc143-mtd.lib>
    $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/lib/Debug/zlibstaticd.lib>
    $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/lib/Release/zlibstatic.lib>
)

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(SHADER_DEST_DIR "$<TARGET_FILE_DIR:duGL>/assets")

add_custom_command(
    TARGET duGL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ARGS ${SHADER_SOURCE_DIR} ${SHADER_DEST_DIR}
    COMMENT "Copying assets directory to output folder."
)