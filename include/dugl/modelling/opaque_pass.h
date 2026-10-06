#pragma once

#include "render_pass.h"


class OpaquePass : public RenderPass
{
public:
    bool accepts(const Entity& entity) const override;
    void render(std::span<Entity* const> entities) override;
};
