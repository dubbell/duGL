#pragma once

#include "dugl/modelling/primitive_builder.h"
#include "dugl/physics/physics_material.h"
#include "dugl/physics/ragdoll_builder.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Ragdoll/Ragdoll.h>


/* Create the JPH body settings so that the primitive can be simulated in the physics system. */
JPH::BodyCreationSettings createBodySettings(const PrimitiveBuilder& builder, const PhysicsMaterial& material);
/* Create the JPH ragdoll settings so that the ragdoll's parts and joints can be simulated in the physics system. */
JPH::Ref<JPH::RagdollSettings> createRagdollSettings(const RagdollBuilder& builder);
