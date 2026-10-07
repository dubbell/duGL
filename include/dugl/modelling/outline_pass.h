#pragma once

#include "render_pass.h"
#include "dugl/shading/shader.h"


class OutlinePass : public RenderPass
{
private:
    Shader* outlineShader;
    std::uint64_t acceptedFlags;

    float outlineThickness;

public:
    OutlinePass(Shader* outlineShader, std::uint64_t acceptedFlags);

    bool accepts(const Entity& entity) const override;
    void render(std::span<Entity* const> entities) override;

    void setOutlineShader(Shader* outlineShader);

    void setOutlineThickness(float outlineThickness);
    float getOutlineThickness() const;
};
