#include <entity.h>



Entity::Entity(Renderable* renderable, glm::vec3 &position) 
    : _renderable(renderable), _position(position) 
{};

glm::vec3 Entity::getPosition() const
{
    return _position;
}

glm::mat4 Entity::getModelTransform() const
{
    glm::mat4 model(1.0f);
    return glm::translate(model, _position);
}

Shader* Entity::getShader() const
{
    return _renderable->getShader();
}

const Material& Entity::getMaterial() const
{
    return _renderable->getMaterial();
}

void Entity::render() const
{
    _renderable->getShader()->setMat4("model", getModelTransform());
    _renderable->render();
}