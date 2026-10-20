#include "dugl/physics/ragdoll_entity.h"
#include "dugl/physics/body_settings.h"
#include "dugl/utils/jolt_adapter.h"

#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>


static glm::mat4 toTransform(glm::vec3 position, glm::quat rotation)
{
    return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation);
}

RagdollEntity::RagdollEntity(Renderable* renderable, Shader* shader, JPH::PhysicsSystem& physicsSystem, RagdollBuilder& builder)
    : Entity(renderable, shader),
      physicsSystem(physicsSystem),
      bodyInterface(physicsSystem.GetBodyInterface()),
      ragdoll(createRagdollSettings(builder)->CreateRagdoll(nextCollisionGroup++, 0, &physicsSystem)),
      localRayHitTests(builder.createLocalRayHitTests()),
      constraints(physicsSystem)
{
    if (ragdoll == nullptr) {
        throw std::runtime_error("Failed to create ragdoll: out of bodies.");
    }

    for (const glm::mat4& restTransform : builder.getRestTransforms()) {
        inverseRestTransforms.push_back(glm::inverse(restTransform));
    }
    partTransforms.resize(inverseRestTransforms.size());
    boneMatrices.resize(inverseRestTransforms.size());

    ragdoll->AddToPhysicsSystem(JPH::EActivation::Activate);
    syncTransforms();
}

RagdollEntity::~RagdollEntity()
{
    constraints.clear();
    ragdoll->RemoveFromPhysicsSystem();
}

void RagdollEntity::preUpdate(float dt)
{
    syncTransforms();
}

void RagdollEntity::syncTransforms()
{
    for (size_t i = 0; i < partTransforms.size(); i++)
    {
        JPH::RVec3 partPosition;
        JPH::Quat partRotation;
        bodyInterface.GetPositionAndRotation(ragdoll->GetBodyID((int)i), partPosition, partRotation);
        partTransforms[i] = toTransform(toGlm(partPosition), toGlm(partRotation));
    }

    JPH::BodyID rootId = ragdoll->GetBodyID(0);
    position = glm::vec3(partTransforms[0][3]);
    rotation = glm::quat_cast(glm::mat3(partTransforms[0]));
    velocity = toGlm(bodyInterface.GetLinearVelocity(rootId));
    angularVelocity = toGlm(bodyInterface.GetAngularVelocity(rootId));

    glm::mat4 inverseModel = glm::inverse(getModelTransform());
    for (size_t i = 0; i < boneMatrices.size(); i++) {
        boneMatrices[i] = inverseModel * partTransforms[i] * inverseRestTransforms[i];
    }
}

void RagdollEntity::render(Shader* shader)
{
    shader->setMat4("model", getModelTransform());
    if (shader->hasActiveUniform("bones[0]")) {
        shader->setMat4Array("bones", boneMatrices);
    }
    renderable->render(shader);
}

dugl::uint RagdollEntity::getPartCount() const
{
    return (dugl::uint)partTransforms.size();
}

JPH::BodyID RagdollEntity::getPartBodyId(dugl::uint part) const
{
    return ragdoll->GetBodyID((int)part);
}

glm::mat4 RagdollEntity::getPartTransform(dugl::uint part) const
{
    return partTransforms[part];
}

void RagdollEntity::setPosition(glm::vec3 position)
{
    moveRigidly(toTransform(position, rotation));
}

void RagdollEntity::setRotation(glm::quat rotation)
{
    moveRigidly(toTransform(position, rotation));
}

void RagdollEntity::moveRigidly(const glm::mat4& targetRootTransform)
{
    glm::mat4 delta = targetRootTransform * glm::inverse(getModelTransform());
    for (size_t i = 0; i < partTransforms.size(); i++)
    {
        glm::mat4 partTransform = delta * partTransforms[i];
        bodyInterface.SetPositionAndRotation(
            ragdoll->GetBodyID((int)i),
            toJolt(glm::vec3(partTransform[3])),
            toJolt(glm::normalize(glm::quat_cast(glm::mat3(partTransform)))),
            JPH::EActivation::Activate);
    }
    syncTransforms();
}

void RagdollEntity::setVelocity(glm::vec3 velocity)
{
    Entity::setVelocity(velocity);
    ragdoll->SetLinearVelocity(toJolt(velocity));
}

void RagdollEntity::setAngularVelocity(glm::vec3 angularVelocity)
{
    Entity::setAngularVelocity(angularVelocity);
    ragdoll->SetLinearAndAngularVelocity(toJolt(velocity), toJolt(angularVelocity));
}

void RagdollEntity::applyImpulse(glm::vec3 impulse)
{
    ragdoll->AddImpulse(toJolt(impulse));
}

void RagdollEntity::update(float dt)
{
    bool held = hasFlag(EntityFlag::Held);
    if (holdConstraint && !held) {
        constraints.remove(*holdConstraint);
        holdConstraint.reset();
    }

    if (!held) {
        return;
    }

    glm::vec3 heldInWorldSpace = getProperty(EntityProperty::HeldInWorldSpace, position);

    if (holdConstraint) {
        constraints.setAnchor(*holdConstraint, heldInWorldSpace);
    }
    else if (hitPart)
    {
        glm::vec3 grabPoint = position + rotation * getProperty(EntityProperty::HeldInEntitySpace, glm::vec3(0.0f));
        glm::vec3 grabPointInPartSpace = glm::vec3(glm::inverse(partTransforms[*hitPart]) * glm::vec4(grabPoint, 1.0f));
        holdConstraint = constraints.addPointConstraint(ragdoll->GetBodyID((int)*hitPart), {
            .pointInEntitySpace = grabPointInPartSpace,
            .anchorInWorldSpace = heldInWorldSpace,
            .frequency = HOLD_FREQUENCY,
            .damping = HOLD_DAMPING });
    }
}

std::optional<float> RagdollEntity::intersectRay(const Ray& ray) const
{
    std::optional<float> nearest;
    std::optional<dugl::uint> nearestPart;

    for (size_t i = 0; i < partTransforms.size(); i++)
    {
        glm::mat4 toPartSpace = glm::inverse(partTransforms[i]);
        glm::vec3 origin = glm::vec3(toPartSpace * glm::vec4(ray.origin, 1.0f));
        glm::vec3 direction = glm::mat3(toPartSpace) * ray.direction;

        std::optional<float> hit = localRayHitTests[i](origin, direction);
        if (hit && (!nearest || *hit < *nearest)) {
            nearest = hit;
            nearestPart = (dugl::uint)i;
        }
    }

    hitPart = nearestPart;
    return nearest;
}
