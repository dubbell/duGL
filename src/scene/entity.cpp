#include <entity.h>



Entity::Entity(Renderable* renderable, glm::vec3 &position) 
    : _renderable(renderable), _position(position) 
{};

glm::vec3 Entity::getPosition()
{
    return _position;
}

glm::mat4 Entity::getModelTransform()
{
    glm::mat4 model(1.0f);
    return glm::translate(model, _position);
}

void Entity::render()
{
    _renderable->getShader()->setMatrix("model", getModelTransform());
    _renderable->render();
}