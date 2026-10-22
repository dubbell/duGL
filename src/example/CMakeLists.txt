add_library(example STATIC "example_env.cpp" "example_physics_env.cpp" "height_field_env.cpp" "draggable_entity.cpp" "draggable_ragdoll_entity.cpp")
target_include_directories(example PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(example
	PUBLIC  env physics
	PRIVATE imgui)
