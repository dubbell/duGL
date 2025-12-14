#include <renderable.h>


Renderable::Renderable(GLint startIndex, GLsizei vertexCount, unsigned int VAO, unsigned int VBO, IVertexManager* vertexManager, Shader* shader)
    : _startIndex(startIndex), _vertexCount(vertexCount), _VAO(VAO), _VBO(VBO), _vertexManager(vertexManager), _shader(shader)
{}

// getters ---------------

GLint Renderable::getStartIndex() const
{
    return _startIndex;
}

GLsizei Renderable::getVertexCount() const
{
    return _vertexCount;
}

unsigned int Renderable::getVAO() const
{
    return _VAO;
}

unsigned int Renderable::getVBO() const
{
    return _VBO;
}

Shader* Renderable::getShader() const
{
    return _shader;
}

const Material& Renderable::getMaterial() const
{
    return _material;
}



// setters ---------------

void Renderable::setStartIndex(GLint startIndex)
{
    _startIndex = startIndex;
}

void Renderable::setShader(Shader* shader)
{
    _shader = shader;
}

void Renderable::setMaterial(Material material)
{
    _material = material;
}

// functional ----------

void Renderable::render() const
{
    if (!_vertexManager->isLoaded(_VAO, _VBO))
    {
        std::cout << "Error: Renderable not loaded." << std::endl;
        return;
    }
    
    glDrawArrays(GL_TRIANGLES, _startIndex, _vertexCount);
}