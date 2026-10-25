#pragma once

#include "render_pass.h"
#include "dugl/shading/shader.h"
#include "dugl/shading/framebuffer.h"


class ShadowPass : public RenderPass
{
public:
    static constexpr int SHADOW_MAP_UNIT = 2;

private:
    Shader depthShader;
    Shader skinnedDepthShader;
    Framebuffer shadowMap;

    glm::vec3 lightDirection;
    glm::vec3 boundsCenter;
    float boundsRadius;
    glm::mat4 lightSpace;

public:
    ShadowPass();

    ShadowPass(const ShadowPass&) = delete;
    ShadowPass& operator=(const ShadowPass&) = delete;

    bool accepts(const Entity& entity) const override;
    void render(std::span<Entity* const> entities) override;

    void setLightDirection(glm::vec3 lightDirection);
    void setBounds(glm::vec3 center, float radius);
    glm::mat4 getLightSpace() const;
    float getTexelWorldSize() const;

private:
    void updateLightSpace();
};
