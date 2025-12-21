#include "uniform_buffer.h"


UniformBuffer::UniformBuffer() {}

void UniformBuffer::create(const char* uboName, std::vector<Shader*> shaders, GLsizeiptr bufferSize, GLenum usage)
{
    this->bufferSize = bufferSize;

    for (auto shader : shaders)
    {
        // get block index of `uboName`
        unsigned int uniformBlockIndex = glGetUniformBlockIndex(shader->ID, uboName);
        // bind the block index to the `bufferCount` binding point
        glUniformBlockBinding(shader->ID, uniformBlockIndex, bufferCount);
    }

    // create uniform buffer object
    glGenBuffers(1, &UBO);

    // allocate memory
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, usage);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, bufferCount, UBO);

    bufferCount++; // increment bufferCount for next UBO
}