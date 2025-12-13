#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <map>

#include <vmanager_interface.h>
#include <shader.h>


struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

class Renderable
{
private:
    GLint _startIndex;
    GLsizei _vertexCount;
    unsigned int _VAO, _VBO;

    IVertexManager* _vertexManager;
    Shader* _shader;

    std::vector<unsigned int> textures;
    Material _material;

public:
    Renderable(GLint startIndex, GLsizei vertexCount, unsigned int VAO, unsigned int VBO, IVertexManager* vertexManager, Shader* shader);

    GLint getStartIndex() const;
    GLsizei getVertexCount() const;
    unsigned int getVAO() const;
    unsigned int getVBO() const;
    Shader* getShader() const;
    const std::vector<unsigned int>& getTextures() const;
    const Material& getMaterial() const;
    
    void setStartIndex(GLint startIndex);
    void setShader(Shader* shader);
    void addTexture(unsigned int texture);
    void setMaterial(Material material);

    void render() const;
};


#endif