add_library(controllers STATIC keyboard.cpp mouse.cpp flight_controller.cpp)
target_link_libraries(controllers
	PUBLIC  glad glfw glm::glm
	PRIVATE view)
target_include_directories(controllers PUBLIC ${PROJECT_SOURCE_DIR}/include)