add_library(view STATIC camera.cpp)
target_link_libraries(view PUBLIC glfw glm::glm)
target_include_directories(view PUBLIC ${PROJECT_SOURCE_DIR}/include)