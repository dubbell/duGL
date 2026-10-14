#include "dugl/modelling/entity.h"
#include "dugl/common.h"

#include <glm/gtc/matrix_transform.hpp>


template <typename T>
static T findProperty(const std::unordered_map<dugl::uint, EntityPropertyValue>& properties, dugl::uint property, T defaultValue)
{
    auto it = properties.find(property);
    if (it == properties.end()) {
        return defaultValue;
    }

    const T* value = std::get_if<T>(&it->second);
    return value ? *value : defaultValue;
}

Entity::Entity(Renderable* renderable, Shader* shader)
    : renderable(renderable), shader(dugl::requireNonNull(shader)), position(0.0f, 0.0f, 0.0f) {}


Entity::Entity(Renderable* renderable, Shader* shader, glm::vec3 position)
    : renderable(renderable), shader(dugl::requireNonNull(shader)), position(position) {}


void Entity::tick(const EntityUpdateContext& context)
{
    preUpdate(context.dt);
    updateState(context);
    update(context.dt);
}

void Entity::updateState(const EntityUpdateContext& context)
{
    // Hover state.
    const std::optional<Ray>& ray = context.hoverRay;
    float hoverRadius = getProperty(EntityProperty::HoverRadius, 1.0f);
    bool hovered = ray && hasFeature(EntityFeature::Hoverable) && 
        checkRayIntersection(position, hoverRadius, ray->origin, ray->direction);
    setFlag(EntityFlag::Hovered, hovered);

    // Selection state.
    if (hasFeature(EntityFeature::Selectable))
    {
        if (!hovered) {
            selectArmed = false;
        } else if (context.leftMousePressed) {
            selectArmed = true;
        }

        if (ray && context.leftMouseReleased) {
            setFlag(EntityFlag::Selected, selectArmed);
            selectArmed = false;
        }
    }

    // Hold state.
    if (hovered && hasFeature(EntityFeature::Holdable) && context.leftMousePressed)
    {
        std::optional<glm::vec3> heldPoint = intersectRaySphere(position, hoverRadius, ray->origin, ray->direction);
        if (heldPoint) {
            setProperty(EntityProperty::HeldInEntitySpace, glm::inverse(rotation) * (*heldPoint - position));
            setProperty(EntityProperty::HeldInWorldSpace, *heldPoint);
            setFlag(EntityFlag::Held, true);
        }
    }
    else if (hasFlag(EntityFlag::Held) && (context.leftMouseReleased || !hasFeature(EntityFeature::Holdable))) 
    {
        setFlag(EntityFlag::Held, false);
        removeProperty(EntityProperty::HeldInEntitySpace);
        removeProperty(EntityProperty::HeldInWorldSpace);
    } 
    else if (hasFlag(EntityFlag::Held) && ray)
    {
        glm::vec3 heldPoint = getProperty(EntityProperty::HeldInWorldSpace, position);
        std::optional<glm::vec3> draggedPoint = intersectRayPlane(heldPoint, context.cameraFront, ray->origin, ray->direction);
        if (draggedPoint) {
            setProperty(EntityProperty::HeldInWorldSpace, *draggedPoint);
        }
    }
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
    } else {
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

void Entity::setFeature(dugl::uint feature, bool value)
{
    if (value) {
        features |= std::uint64_t{1} << feature;
    } else {
        features &= ~(std::uint64_t{1} << feature);
    }
}

bool Entity::hasFeature(dugl::uint feature) const
{
    return (features >> feature & 1) != 0;
}

bool Entity::hasAnyFeature(std::uint64_t mask) const
{
    return (features & mask) != 0;
}

void Entity::setProperty(dugl::uint property, float value)
{
    properties[property] = value;
}

void Entity::setProperty(dugl::uint property, glm::vec3 value)
{
    properties[property] = value;
}

float Entity::getProperty(dugl::uint property, float defaultValue) const
{
    return findProperty(properties, property, defaultValue);
}

glm::vec3 Entity::getProperty(dugl::uint property, glm::vec3 defaultValue) const
{
    return findProperty(properties, property, defaultValue);
}

void Entity::removeProperty(dugl::uint property)
{
    properties.erase(property);
}

void Entity::setPosition(glm::vec3 position)
{
    this->position = position;
}


glm::vec3 Entity::getPosition() const
{
    return position;
}

void Entity::setRotation(glm::quat rotation)
{
    this->rotation = rotation;
}

glm::quat Entity::getRotation() const
{
    return rotation;
}

void Entity::setVelocity(glm::vec3 velocity)
{
    this->velocity = velocity;
}

glm::vec3 Entity::getVelocity() const
{
    return velocity;
}

void Entity::setAngularVelocity(glm::vec3 angularVelocity)
{
    this->angularVelocity = angularVelocity;
}

glm::vec3 Entity::getAngularVelocity() const
{
    return angularVelocity;
}

glm::mat4 Entity::getModelTransform()
{
    return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation);
}
