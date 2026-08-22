add_library(env STATIC "env.cpp" "simple_env.cpp" "jolt_env.cpp")
target_include_directories(env PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(env
	PUBLIC  glad glfw glm::glm imgui async physics
	PRIVATE stb assimp)