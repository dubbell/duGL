add_library(application STATIC game.cpp)
target_include_directories(application PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(application
	PUBLIC  glad glfw glm::glm imgui 
	PRIVATE stb assimp)

add_subdirectory(controllers)
add_subdirectory(shading)
add_subdirectory(view)
add_subdirectory(modelling)
