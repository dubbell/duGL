#include <renderable.h>



Renderable::Renderable(GLint startIndex, GLsizei vertexCount)
    : startIndex(startIndex), vertexCount(vertexCount)
{}

GLint Renderable::getStartIndex()
{
    return startIndex;
}

GLsizei Renderable::getVertexCount()
{
    return vertexCount;
}