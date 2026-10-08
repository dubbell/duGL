#include "dugl/physics/common.h"

#include <utility>

using JPH::uint;
using JPH::uint64;
using JPH::BroadPhaseLayer;
using JPH::ObjectLayer;
using JPH::Body;
using JPH::BodyID;
using JPH::RVec3;
using JPH::ContactManifold;
using JPH::ContactSettings;
using JPH::SubShapeIDPair;


ObjectToBroadPhaseMapper::ObjectToBroadPhaseMapper()
    {
        objectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        objectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

uint ObjectToBroadPhaseMapper::GetNumBroadPhaseLayers() const
{
    return BroadPhaseLayers::NUM_LAYERS;
}

BroadPhaseLayer ObjectToBroadPhaseMapper::GetBroadPhaseLayer(ObjectLayer objectLayer) const
{
    return objectToBroadPhase[objectLayer];
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char* ObjectToBroadPhaseMapper::GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const
{
    switch ((JPH::BroadPhaseLayer::Type)inLayer) {
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:     return "MOVING";
        default:                                                       return "INVALID";
    }
}
#endif


bool ObjectCollisionFilter::ShouldCollide(ObjectLayer objectLayer1, ObjectLayer objectLayer2) const
{
    switch (objectLayer1) {
        case Layers::NON_MOVING:
            return objectLayer2 == Layers::MOVING;
        case Layers::MOVING:
            return true;
        default:
            return false;
    }
}


bool ObjectBroadPhaseFilter::ShouldCollide(ObjectLayer objectLayer, BroadPhaseLayer broadPhaseLayer) const
{
    switch (objectLayer) {
        case Layers::NON_MOVING:
            return broadPhaseLayer == BroadPhaseLayers::MOVING;
        case Layers::MOVING:
            return true;
        default:
            return false;
    }
}


static uint64 bodyPairKey(const BodyID& body1, const BodyID& body2)
{
    return (uint64(body1.GetIndexAndSequenceNumber()) << 32) | body2.GetIndexAndSequenceNumber();
}

void ContactRecorder::OnContactAdded(const Body& body1, const Body& body2, const ContactManifold& manifold, ContactSettings& settings)
{
    std::lock_guard lock(mutex);

    if (contactCounts[bodyPairKey(body1.GetID(), body2.GetID())]++ > 0) {
        return;
    }

    uint numPoints = manifold.mRelativeContactPointsOn1.size();
    RVec3 point = RVec3::sZero();
    for (uint point_i = 0; point_i < numPoints; point_i++) {
        point += manifold.GetWorldSpaceContactPointOn1(point_i);
    }
    point /= float(numPoints);

    contacts.push_back({
        CollisionPhase::Begin,
        body1.GetID(),
        body2.GetID(),
        point,
        manifold.mWorldSpaceNormal,
        manifold.mPenetrationDepth,
        body2.GetPointVelocity(point) - body1.GetPointVelocity(point) });
}

void ContactRecorder::OnContactRemoved(const SubShapeIDPair& subShapePair)
{
    std::lock_guard lock(mutex);

    auto it = contactCounts.find(bodyPairKey(subShapePair.GetBody1ID(), subShapePair.GetBody2ID()));
    if (it == contactCounts.end() || --it->second > 0) {
        return;
    }

    contactCounts.erase(it);
    contacts.push_back({ CollisionPhase::End, subShapePair.GetBody1ID(), subShapePair.GetBody2ID() });
}

std::vector<RecordedContact> ContactRecorder::drain()
{
    std::lock_guard lock(mutex);
    return std::exchange(contacts, {});
}