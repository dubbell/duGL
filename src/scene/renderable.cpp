#include <renderable.h>



Renderable::Renderable(int startIndex, int vertexCount)
    : startIndex(startIndex), vertexCount(vertexCount)
{}

int Renderable::getStartIndex()
{
    return startIndex;
}

int Renderable::getVertexCount()
{
    return vertexCount;
}