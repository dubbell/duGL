#pragma once

#include "dugl/shading/shader.h"
#include "dugl/modelling/renderable.h"
#include "dugl/common.h"
#include "dugl/utils/common.h"

#include <glm/gtc/quaternion.hpp>

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <variant>


namespace EntityFlag
{
    enum : dugl::uint
    {
        Hovered = 0,
        Selected = 1,
        Held = 2,
        UserBegin = 32
    };
}

namespace EntityFeature
{
    enum : dugl::uint
    {
        Hoverable = 0,
        Selectable = 1,
        Holdable = 2,
        UserBegin = 32
    };
}

namespace EntityProperty
{
    enum : dugl::uint
    {
        HoverRadius = 0,
        HeldInEntitySpace = 1,
        HeldInWorldSpace = 2,
        UserBegin = 32
    };
}

using EntityPropertyValue = std::variant<float, glm::vec3>;

template <typename... Flags>
constexpr std::uint64_t flagMask(Flags... flags)
{
    return ((std::uint64_t{1} << flags) | ...);
}

struct EntityUpdateContext
{
    float dt;
    std::optional<Ray> hoverRay;
    glm::vec3 cameraFront{ 0.0f };
    bool leftMousePressed = false;
    bool leftMouseReleased = false;
};

DUGL_NAMESPACE_BEGIN
class EntityUpdateJob;
DUGL_NAMESPACE_END

// An Entity is the world space representation of a Renderable. While
// each Renderable is singular, multiple Entity objects with the same
// Renderable can exist and be rendered on the screen simultaneously.
class Entity
{
    friend class dugl::EntityUpdateJob;

protected:
    Renderable* renderable;
    Shader* shader;

    glm::vec3 position;
    glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec3 velocity{ 0.0f };
    glm::vec3 angularVelocity{ 0.0f };

    std::uint64_t flags = 0;
    std::uint64_t features = 0;
    std::unordered_map<dugl::uint, EntityPropertyValue> properties;

private:
    bool selectArmed = false;

public:
    Entity(Renderable* model, Shader* shader);
    Entity(Renderable* model, Shader* shader, glm::vec3 position);
    virtual ~Entity() = default;

    virtual void render(Shader* shader);

    void setShader(Shader* shader);
    Shader* getShader() const;

    void setFlag(dugl::uint flag, bool value);
    bool hasFlag(dugl::uint flag) const;
    bool hasAnyFlag(std::uint64_t mask) const;

    void setFeature(dugl::uint feature, bool value);
    bool hasFeature(dugl::uint feature) const;
    bool hasAnyFeature(std::uint64_t mask) const;

    void setProperty(dugl::uint property, float value);
    void setProperty(dugl::uint property, glm::vec3 value);
    float getProperty(dugl::uint property, float defaultValue) const;
    glm::vec3 getProperty(dugl::uint property, glm::vec3 defaultValue) const;
    void removeProperty(dugl::uint property);

    void setPosition(glm::vec3 position);
    glm::vec3& getPosition();

    void setRotation(glm::quat rotation);
    glm::quat& getRotation();

    void setVelocity(glm::vec3 velocity);
    glm::vec3& getVelocity();

    void setAngularVelocity(glm::vec3 angularVelocity);
    glm::vec3& getAngularVelocity();

    glm::mat4 getModelTransform();

protected:
    virtual void preUpdate(float dt) {}
    virtual void update(float dt) {}

private:
    void tick(const EntityUpdateContext& context);
    void updateState(const EntityUpdateContext& context);
};
