#include "dugl/modelling/entity.h"
#include "dugl/common.h"

#include <glm/gtc/matrix_transform.hpp>


Entity::Entity(Renderable* renderable, Shader* shader)
    : renderable(renderable), shader(dugl::requireNonNull(shader)), position(0.0f, 0.0f, 0.0f) {}


Entity::Entity(Renderable* renderable, Shader* shader, glm::vec3 position)
    : renderable(renderable), shader(dugl::requireNonNull(shader)), position(position) {}


void Entity::tick(const EntityUpdateContext& context)
{
    update(context.dt);

    const std::optional<Ray>& ray = context.hoverRay;
    bool hovered = ray && hasFlag(EntityFlag::Hoverable) && checkRayIntersection(
        position, getProperty(EntityProperty::HoverRadius, 1.0f), ray->origin, ray->direction);
    setFlag(EntityFlag::Hovered, hovered);
}

void Entity::render(Shader* shader)
{
    shader->setMat4("model", getModelTransform());
    renderable->render(shader);
}

void Entity::setShader(Shader* shader)
{
    this->shader = dugl::requireNonNull(shader);
}

Shader* Entity::getShader() const
{
    return shader;
}

void Entity::setFlag(dugl::uint flag, bool value)
{
    if (value) {
        flags |= std::uint64_t{1} << flag;
    }
    else {
        flags &= ~(std::uint64_t{1} << flag);
    }
}

bool Entity::hasFlag(dugl::uint flag) const
{
    return (flags >> flag & 1) != 0;
}

bool Entity::hasAnyFlag(std::uint64_t mask) const
{
    return (flags & mask) != 0;
}

void Entity::setProperty(dugl::uint property, float value)
{
    properties[property] = value;
}

float Entity::getProperty(dugl::uint property, float defaultValue) const
{
    auto it = properties.find(property);
    return it == properties.end() ? defaultValue : it->second;
}

void Entity::removeProperty(dugl::uint property)
{
    properties.erase(property);
}

void Entity::setPosition(glm::vec3 position)
{
    this->position = position;
}


glm::vec3& Entity::getPosition()
{
    return position;
}

void Entity::setRotation(glm::quat rotation)
{
    this->rotation = rotation;
}

glm::quat& Entity::getRotation()
{
    return rotation;
}

void Entity::setVelocity(glm::vec3 velocity)
{
    this->velocity = velocity;
}

glm::vec3& Entity::getVelocity()
{
    return velocity;
}

void Entity::setAngularVelocity(glm::vec3 angularVelocity)
{
    this->angularVelocity = angularVelocity;
}

glm::vec3& Entity::getAngularVelocity()
{
    return angularVelocity;
}

glm::mat4 Entity::getModelTransform()
{
    return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation);
}
