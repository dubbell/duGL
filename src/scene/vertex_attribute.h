#ifndef VERTEX_ATTRIBUTE_H
#define VERTEX_ATTRIBUTE_H

#include <glad/gl.h>

class VertexAttribute
{
public:
    VertexAttribute(GLuint size, GLenum type, GLboolean normalized)
        : size(size), type(type), normalized(normalized) {};

    GLuint size;
    GLenum type;
    GLboolean normalized;
};

#endif