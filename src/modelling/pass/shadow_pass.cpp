#include "dugl/modelling/shadow_pass.h"
#include "dugl/utils/glad_include.h"

#include <glm/gtc/matrix_transform.hpp>


static constexpr int SHADOW_MAP_SIZE = 2048;

ShadowPass::ShadowPass()
    : depthShader("assets/shaders/shadow_depth.vert", "assets/shaders/shadow_depth.frag"),
      skinnedDepthShader("assets/shaders/skinned_shadow_depth.vert", "assets/shaders/shadow_depth.frag"),
      shadowMap({
          { GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32F, 0,
            { GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, glm::vec4(1.0f), GL_COMPARE_REF_TO_TEXTURE, GL_LEQUAL } } }),
      lightDirection(0.0f, -1.0f, 0.0f), boundsCenter(0.0f), boundsRadius(25.0f), lightSpace(1.0f)
{
    shadowMap.resize(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    updateLightSpace();
}

bool ShadowPass::accepts(const Entity& entity) const
{
    return true;
}

void ShadowPass::render(std::span<Entity* const> entities)
{
    {
        FramebufferScope scope(shadowMap);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.0f, 4.0f);

        depthShader.setMat4("lightSpace", lightSpace);
        skinnedDepthShader.setMat4("lightSpace", lightSpace);

        Shader* activeShader = nullptr;
        for (Entity* entity : entities)
        {
            Shader* shader = isSkinned(*entity) ? &skinnedDepthShader : &depthShader;
            if (shader != activeShader) {
                shader->use();
                activeShader = shader;
            }
            entity->render(shader);
        }
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_UNIT);
    glBindTexture(GL_TEXTURE_2D, shadowMap.getImage(0));
    glActiveTexture(GL_TEXTURE0);
}

void ShadowPass::setLightDirection(glm::vec3 lightDirection)
{
    this->lightDirection = glm::normalize(lightDirection);
    updateLightSpace();
}

void ShadowPass::setBounds(glm::vec3 center, float radius)
{
    boundsCenter = center;
    boundsRadius = radius;
    updateLightSpace();
}

glm::mat4 ShadowPass::getLightSpace() const
{
    return lightSpace;
}

float ShadowPass::getTexelWorldSize() const
{
    return 2.0f * boundsRadius / SHADOW_MAP_SIZE;
}

void ShadowPass::updateLightSpace()
{
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    if (glm::abs(glm::dot(lightDirection, up)) > 0.99f) {
        up = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::mat4 view = glm::lookAt(boundsCenter - lightDirection * boundsRadius, boundsCenter, up);
    glm::mat4 projection = glm::ortho(-boundsRadius, boundsRadius, -boundsRadius, boundsRadius, 0.0f, 2.0f * boundsRadius);
    lightSpace = projection * view;
}
