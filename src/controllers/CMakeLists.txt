add_library(controllers STATIC keyboard.cpp mouse.cpp flight_controller.cpp)
target_link_libraries(controllers PRIVATE view glad glfw glm::glm)
target_include_directories(controllers PUBLIC ${PROJECT_SOURCE_DIR}/include)