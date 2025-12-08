#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include <vertex_attribute.h>


class Renderable
{
private:
    GLint startIndex;
    GLsizei vertexCount;

public:
    Renderable(GLint startIndex, GLsizei vertexCount);

    GLint getStartIndex();
    GLsizei getVertexCount();
};


#endif