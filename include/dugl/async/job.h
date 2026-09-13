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
	float dt;
	std::vector<Entity*> entities;

public:
	EntityUpdateJob(float dt, Entity* entity) : dt(dt), entities{entity} {}
	EntityUpdateJob(float dt, std::vector<Entity*> entities) : dt(dt), entities(std::move(entities)) {}

	void execute() override {
		for (auto& entity : entities) {
			entity->update(dt);
		}
	}
};

DUGL_NAMESPACE_END