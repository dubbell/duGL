add_library(modelling STATIC 
	skybox.cpp 
	entity/entity.cpp entity/outlined_entity.cpp
	renderable/mesh.cpp renderable/model_builder.cpp renderable/renderable.cpp)
target_link_libraries(modelling 
	PUBLIC  assimp glfw
	PRIVATE shading stb)
target_include_directories(modelling PUBLIC ${PROJECT_SOURCE_DIR}/include)