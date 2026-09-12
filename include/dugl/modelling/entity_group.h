#pragma once

#include <vector>

#include "entity.h"


// A collection of Entity objects rendered together in the same kind of render
// pass. Entity groups are responsible for calling use() on shaders before the
// entities are rendered.
class EntityGroup
{
public:
    virtual ~EntityGroup() = default;

    // Render the entities in the group.
    virtual void render() = 0;
};
