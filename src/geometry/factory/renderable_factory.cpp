#include <renderable_factory.h>



std::unique_ptr<CubeBuilder> RenderableFactory::cubeBuilder(VertexManager* vertexManager)
{
    return std::make_unique<CubeBuilder>(vertexManager);
}

std::unique_ptr<GroundBuilder> RenderableFactory::groundBuilder(VertexManager* vertexManager)
{
    return std::make_unique<GroundBuilder>(vertexManager);
}