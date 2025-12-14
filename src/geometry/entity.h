#ifndef ENTITY_H
#define ENTITY_H

#include <renderable.h>
#include <shader.h>


class Entity
{
private:
    Renderable* _renderable;
    glm::vec3 _position;

public:
    Entity(Renderable* renderable, glm::vec3 &position);

    glm::vec3 getPosition() const;
    glm::mat4 getModelTransform() const;
    Shader* getShader() const;
    const Material& getMaterial() const;

    void render() const;
};


#endif