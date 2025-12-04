cmake_minimum_required(VERSION 3.20)

project(duGL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

add_executable(${PROJECT_NAME} main.cpp shader.cpp vendor/glad/src/gl.c stb_image.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm
)

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/lib-vc2022/glfw3.lib
    
    opengl32.lib
    user32.lib
    gdi32.lib
    shell32.lib
)

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(SHADER_DEST_DIR "$<TARGET_FILE_DIR:duGL>/assets")

add_custom_command(
    TARGET duGL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ARGS ${SHADER_SOURCE_DIR} ${SHADER_DEST_DIR}
    COMMENT "Copying shaderse directory to output folder."
)