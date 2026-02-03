#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <string>
#include <vector>
#include <stdexcept>

#include "shader.h"




class UniformBuffer
{
public:
    UniformBuffer();
    
    void create(const char* uboName, std::vector<Shader*> shaders, GLsizeiptr bufferSize, GLenum usage);

    template <typename T>
    void writeData(const T& data)
    {
        if (UBO == 0) throw std::runtime_error("UBO not initialized...");

        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize, &data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

private:
    static inline size_t bufferCount = 0;

    unsigned int UBO;
    GLsizeiptr bufferSize;
};

#endif