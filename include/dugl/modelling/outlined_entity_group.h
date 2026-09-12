#pragma once

#include <vector>

#include "entity_group.h"
#include "hoverable_entity.h"
#include "dugl/shading/shader.h"


// Renders its entities, outlining the ones the cursor is over. Its entities must
// be hoverable, since that is what decides which of them are outlined.
class OutlinedEntityGroup : public EntityGroup
{
private:
    std::vector<HoverableEntity*> hoverableEntities;

    Shader* shader;
    Shader* outlineShader;

    float outlineThickness;

public:
    OutlinedEntityGroup(Shader* shader, Shader* outlineShader);

    void render() override;

    void addEntity(HoverableEntity* entity);
    void removeEntity(HoverableEntity* entity);
    void clear();

    const std::vector<HoverableEntity*>& getHoverableEntities() const;

    void setShader(Shader* shader);
    void setOutlineShader(Shader* outlineShader);

    void setOutlineThickness(float outlineThickness);
    float getOutlineThickness() const;
};
