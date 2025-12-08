#ifndef RENDER_COLLECTION_H
#define RENDER_COLLECTION_H

#include <renderable.h>
#include <vertex_attribute.h>
#include <entity.h>
#include <shader.h>
#include <camera.h>

#include <vector>
#include <map>
#include <memory>


class RenderCollection
{
private:
    // vertex array and vertex buffer objects in OpenGL
    unsigned int VAO, VBO;

    Shader* shader;

    // attributes of all vertex data
    std::vector<VertexAttribute> attributes;
    GLenum usage;
    GLsizei vertexStride;
    GLint vertexHead;

    // vertices and vertex attributes
    std::vector<float> vertexData;

    // collections of vertices
    std::map<std::string, Renderable> renderables;
    // instances of renderables
    std::map<std::string, std::vector<Entity>> entities;

public:
    RenderCollection(Shader* shader);

    void refreshAttributePtrs() const;
    void addVertexAttribute(GLuint size, GLenum type, GLboolean normalized);

    void createRenderable(std::string name, std::vector<float> &vertexData, GLsizei newVertexCount);
    void createEntity(std::string renderable, glm::vec3 &position);

    void loadRemote() const;
    void clearLocal();

    void renderEntities(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix);
};


#endif