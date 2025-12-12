#include <renderable_factory.h>



std::unique_ptr<CubeBuilder> RenderableFactory::cubeBuilder(VertexManager* vertexManager)
{
    return std::make_unique<CubeBuilder>(vertexManager);
}