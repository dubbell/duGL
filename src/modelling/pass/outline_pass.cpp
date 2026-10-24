#include "dugl/modelling/outline_pass.h"
#include "dugl/utils/glad_include.h"
#include "dugl/common.h"


static constexpr int MASK_SAMPLES = 4;

OutlinePass::OutlinePass(Shader* maskShader, Shader* outlineShader, std::uint64_t acceptedFlags, std::uint64_t excludedFlags)
    : maskShader(dugl::requireNonNull(maskShader)), outlineShader(dugl::requireNonNull(outlineShader)),
      acceptedFlags(acceptedFlags), excludedFlags(excludedFlags), outlineThickness(3.0f), depthBias(0.02f), outlineColor(1.0f),
      mask({
          { GL_COLOR_ATTACHMENT0, GL_R8, MASK_SAMPLES },
          { GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, MASK_SAMPLES } }),
      resolve({
          { GL_COLOR_ATTACHMENT0, GL_R8, 0, { GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, glm::vec4(0.0f) } },
          { GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, 0, { GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_BORDER, glm::vec4(1.0f) } } })
{
    glGenVertexArrays(1, &fullscreenVAO);
}

OutlinePass::~OutlinePass()
{
    glDeleteVertexArrays(1, &fullscreenVAO);
}


bool OutlinePass::accepts(const Entity& entity) const
{
    return entity.hasAnyFlag(acceptedFlags) && !entity.hasAnyFlag(excludedFlags);
}

void OutlinePass::render(std::span<Entity* const> entities)
{
    if (entities.empty()) {
        return;
    }

    // Recreate framebuffer attachments if the viewport size has changed.
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int width = viewport[2], height = viewport[3];
    if (width <= 0 || height <= 0) { return; }
    mask.resize(width, height);
    resolve.resize(width, height);

    {
        // Render the entity mask (multisampled).
        FramebufferScope scope(mask);
        const float noCoverage[] = { 0.0f, 0.0f, 0.0f, 0.0f };  // RGBA value for samples that don't hit entities.
        const float farDepth = 1.0f;                            // Maximum depth (far plane).
        glClearBufferfv(GL_COLOR, 0, noCoverage);               // Initially set all sample colors to 0.0
        glClearBufferfv(GL_DEPTH, 0, &farDepth);                // Initially set all sample depths to 1.0

        Shader* activeShader = nullptr;
        for (Entity* entity : entities)
        {
            Shader* shader = getMaskShader(*entity);
            if (shader != activeShader) {
                shader->use();
                activeShader = shader;
            }
            entity->render(shader);
        }

        // Blit the mask into the resolve framebuffer, averaging the coverage samples
        // (depth is not averaged) over each pixel (anti-aliasing).
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mask.getId());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolve.getId());
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
            GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    // Activate the outline shader and set outline parameters.
    outlineShader->use();
    outlineShader->setInt("coverageMask", 0);
    outlineShader->setInt("depthMask", 1);
    outlineShader->setFloat("thickness", outlineThickness);
    outlineShader->setFloat("depthBias", depthBias);
    outlineShader->setVec3("color", outlineColor);

    // Bind the depth and coverage textures from the resolve framebuffer.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, resolve.getImage(1));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, resolve.getImage(0));

    // The outline's alpha becomes per-sample coverage, so its edge resolves with the scene's MSAA.
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    // Draw outlines. Renders a single triangle over the entire screen so that each
    // pixel is processed by the outline fragment shader.
    glBindVertexArray(fullscreenVAO);  // Bind a dummy VAO.
    glDrawArrays(GL_TRIANGLES, 0, 3);  // 3 vertices, constructs the triangle with gl_VertexID in the outline shader.
    glBindVertexArray(0);              // Unbind the dummy VAO.

    // Disable alpha to coverage and unbind textures.
    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OutlinePass::setMaskShader(Shader* maskShader)
{
    this->maskShader = dugl::requireNonNull(maskShader);
}

void OutlinePass::setSkinnedMaskShader(Shader* skinnedMaskShader)
{
    this->skinnedMaskShader = skinnedMaskShader;
}

Shader* OutlinePass::getMaskShader(const Entity& entity) const
{
    return isSkinned(entity) && skinnedMaskShader != nullptr ? skinnedMaskShader : maskShader;
}

void OutlinePass::setOutlineShader(Shader* outlineShader)
{
    this->outlineShader = dugl::requireNonNull(outlineShader);
}

void OutlinePass::setOutlineThickness(float outlineThickness)
{
    this->outlineThickness = outlineThickness;
}

float OutlinePass::getOutlineThickness() const
{
    return outlineThickness;
}

void OutlinePass::setDepthBias(float depthBias)
{
    this->depthBias = depthBias;
}

float OutlinePass::getDepthBias() const
{
    return depthBias;
}

void OutlinePass::setOutlineColor(glm::vec3 outlineColor)
{
    this->outlineColor = outlineColor;
}

glm::vec3 OutlinePass::getOutlineColor() const
{
    return outlineColor;
}
