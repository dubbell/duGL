#include "dugl/physics/common.h"

#include <iostream>

using JPH::uint;
using JPH::BroadPhaseLayer;
using JPH::ObjectLayer;
using JPH::ValidateResult;
using JPH::Body;
using JPH::RVec3Arg;
using JPH::CollideShapeResult;
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


ValidateResult BasicContactListener::OnContactValidate(const Body& body1, const Body& body2, RVec3Arg inBaseOffset, const CollideShapeResult& colShapeResult)
{
	return ValidateResult::AcceptAllContactsForThisBodyPair;
}

void BasicContactListener::OnContactAdded(const Body& body1, const Body& body2, const ContactManifold& manifold, ContactSettings& settings)
{
	std::cout << "A contact was added" << std::endl;
}

void BasicContactListener::OnContactPersisted(const Body& body1, const Body& body2, const ContactManifold& manifold, ContactSettings& settings)
{
	std::cout << "A contact was persisted" << std::endl;
}

void BasicContactListener::OnContactRemoved(const SubShapeIDPair& subShapePair)
{
	std::cout << "A contact was removed" << std::endl;
}