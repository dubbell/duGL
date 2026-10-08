#pragma once

#include "dugl/common.h"
#include "dugl/modelling/entity.h"

#include <vector>


DUGL_NAMESPACE_BEGIN

class Job
{
public:
	virtual ~Job() = default;
	virtual void execute() = 0;
};

class EntityUpdateJob : public Job
{
private:
	const EntityUpdateContext& context;
	std::vector<Entity*> entities;

public:
	EntityUpdateJob(const EntityUpdateContext& context, Entity* entity) : context(context), entities{entity} {}
	EntityUpdateJob(const EntityUpdateContext& context, std::vector<Entity*> entities) : context(context), entities(std::move(entities)) {}

	void execute() override {
		for (auto& entity : entities) {
			entity->tick(context);
		}
	}
};

DUGL_NAMESPACE_END