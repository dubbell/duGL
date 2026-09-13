add_library(env STATIC "env.cpp" "example_env.cpp" "physics_env.cpp" "example_physics_env.cpp")
target_include_directories(env PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(env
	PUBLIC  glad glfw glm::glm view controllers modelling async physics
	PRIVATE imgui)