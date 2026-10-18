#pragma once


enum class MotionType
{
    Static,
    Kinematic,
    Dynamic
};

struct PhysicsMaterial
{
    MotionType motionType = MotionType::Dynamic;
    float friction = 0.2f;
    float restitution = 0.0f;
};
