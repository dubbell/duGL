#ifndef RENDERABLE_FACTORY_H
#define RENDERABLE_FACTORY_H

#include <vector>
#include <string>
#include <memory>

#include <renderable.h>
#include <cube_builder.h>
#include <ground_builder.h>
#include <vertex_manager.h>


class RenderableFactory
{
public:
    static std::unique_ptr<CubeBuilder> cubeBuilder(VertexManager* vertexManager);
    static std::unique_ptr<GroundBuilder> groundBuilder(VertexManager* vertexManager);
};


#endif