#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include <vmanager_interface.h>
#include <shader.h>



class Renderable
{
private:
    GLint _startIndex;
    GLsizei _vertexCount;
    unsigned int _VAO, _VBO;

    IVertexManager* _vertexManager;
    Shader* _shader;

public:
    Renderable(GLint startIndex, GLsizei vertexCount, unsigned int VAO, unsigned int VBO, IVertexManager* vertexManager, Shader* shader);

    GLint getStartIndex();
    void setStartIndex(GLint startIndex);

    GLsizei getVertexCount();

    Shader* getShader();
    void setShader(Shader* shader);

    void render();
};


#endif