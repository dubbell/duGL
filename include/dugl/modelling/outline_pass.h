#pragma once

#include "render_pass.h"
#include "dugl/shading/shader.h"


class OutlinePass : public RenderPass
{
private:
    Shader* maskShader;
    Shader* outlineShader;
    std::uint64_t acceptedFlags;

    float outlineThickness;
    float depthBias;
    glm::vec3 outlineColor;

    // multisampled target the mask is rendered into
    dugl::uint maskFramebuffer = 0;
    dugl::uint maskCoverageBuffer = 0;
    dugl::uint maskDepthBuffer = 0;

    // single-sampled textures the mask is resolved into for sampling
    dugl::uint resolveFramebuffer = 0;
    dugl::uint coverageTexture = 0;
    dugl::uint depthTexture = 0;

    dugl::uint fullscreenVAO = 0;
    int maskWidth = 0;
    int maskHeight = 0;

public:
    OutlinePass(Shader* maskShader, Shader* outlineShader, std::uint64_t acceptedFlags);
    ~OutlinePass();

    OutlinePass(const OutlinePass&) = delete;
    OutlinePass& operator=(const OutlinePass&) = delete;

    bool accepts(const Entity& entity) const override;
    void render(std::span<Entity* const> entities) override;

    void setMaskShader(Shader* maskShader);
    void setOutlineShader(Shader* outlineShader);

    void setOutlineThickness(float outlineThickness);
    float getOutlineThickness() const;

    void setDepthBias(float depthBias);
    float getDepthBias() const;

    void setOutlineColor(glm::vec3 outlineColor);
    glm::vec3 getOutlineColor() const;

private:
    /* (Re-)create the images in the 2 framebuffers if the viewport size changed (or first call to resizeMask). */
    void resizeMask(int width, int height);
};
