add_library(env STATIC "simple_env.cpp" "env.cpp")
target_include_directories(env PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(env
	PUBLIC  glad glfw glm::glm imgui 
	PRIVATE stb assimp)