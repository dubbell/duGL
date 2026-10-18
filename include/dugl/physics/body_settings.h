#pragma once

#include "dugl/modelling/primitive_builder.h"
#include "dugl/physics/physics_material.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>


/* Create the JPH body settings so that the primitive can be simulated in the physics system. */
JPH::BodyCreationSettings createBodySettings(const PrimitiveBuilder& builder, const PhysicsMaterial& material);
