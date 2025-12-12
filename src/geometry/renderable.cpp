#include <renderable.h>


Renderable::Renderable(GLint startIndex, GLsizei vertexCount, unsigned int VAO, unsigned int VBO, IVertexManager* vertexManager, Shader* shader)
    : _startIndex(startIndex), _vertexCount(vertexCount), _VAO(VAO), _VBO(VBO), _vertexManager(vertexManager), _shader(shader)
{}

GLint Renderable::getStartIndex()
{
    return _startIndex;
}

void Renderable::setStartIndex(GLint startIndex)
{
    _startIndex = startIndex;
}

GLsizei Renderable::getVertexCount()
{
    return _vertexCount;
}

Shader* Renderable::getShader()
{
    return _shader;
}

void Renderable::setShader(Shader* shader)
{
    _shader = shader;
}

void Renderable::render()
{
    if (!_vertexManager->isLoaded(_VAO, _VBO))
    {
        std::cout << "Error: Renderable not loaded." << std::endl;
        return;
    }
    
    glDrawArrays(GL_TRIANGLES, _startIndex, _vertexCount);
}