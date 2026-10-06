#pragma once

#include "dugl/shading/shader.h"
#include "dugl/modelling/renderable.h"
#include "dugl/common.h"

#include <glm/gtc/quaternion.hpp>

#include <cstdint>


namespace EntityFlag
{
    enum : dugl::uint
    {
        Outlined = 0,
        UserBegin = 32
    };
}

// An Entity is the world space representation of a Renderable. While
// each Renderable is singular, multiple Entity objects with the same
// Renderable can exist and be rendered on the screen simultaneously.
class Entity
{
protected:
    Renderable* renderable;
    Shader* shader;
    glm::vec3 position;
    glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec3 velocity{ 0.0f };
    glm::vec3 angularVelocity{ 0.0f };
    std::uint64_t flags = 0;

public:
    Entity(Renderable* model, Shader* shader);
    Entity(Renderable* model, Shader* shader, glm::vec3 position);
    virtual ~Entity() = default;

    virtual void update(float dt) {}
    virtual void render(Shader* shader);

    void setShader(Shader* shader);
    Shader* getShader() const;

    void setFlag(dugl::uint flag, bool value);
    bool hasFlag(dugl::uint flag) const;

    void setPosition(glm::vec3 position);
    glm::vec3& getPosition();

    void setRotation(glm::quat rotation);
    glm::quat& getRotation();

    void setVelocity(glm::vec3 velocity);
    glm::vec3& getVelocity();

    void setAngularVelocity(glm::vec3 angularVelocity);
    glm::vec3& getAngularVelocity();

    glm::mat4 getModelTransform();
};
