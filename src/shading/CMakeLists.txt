add_library(shading STATIC shader.cpp uniform_buffer.cpp)
target_include_directories(shading PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(shading PUBLIC glad glfw glm::glm)