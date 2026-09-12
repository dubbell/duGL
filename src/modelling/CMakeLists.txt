add_library(modelling STATIC 
	skybox.cpp 
	entity/entity.cpp entity/hoverable_entity.cpp
	 group/standard_entity_group.cpp group/outlined_entity_group.cpp
	renderable/mesh.cpp renderable/renderable.cpp
	renderable/renderable_builder.cpp renderable/model_builder.cpp renderable/primitive_builder.cpp scene.cpp)
target_link_libraries(modelling
	PUBLIC  assimp
	PRIVATE shading stb controllers)
target_include_directories(modelling PUBLIC ${PROJECT_SOURCE_DIR}/include)