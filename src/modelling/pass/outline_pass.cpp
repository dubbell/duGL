#include "dugl/modelling/outline_pass.h"
#include "dugl/utils/glad_include.h"
#include "dugl/common.h"

#include <stdexcept>


static constexpr int MASK_SAMPLES = 4;

OutlinePass::OutlinePass(Shader* maskShader, Shader* outlineShader, std::uint64_t acceptedFlags)
    : maskShader(dugl::requireNonNull(maskShader)), outlineShader(dugl::requireNonNull(outlineShader)),
      acceptedFlags(acceptedFlags), outlineThickness(3.0f), depthBias(0.02f), outlineColor(1.0f)
{
    glGenFramebuffers(1, &maskFramebuffer);
    glGenFramebuffers(1, &resolveFramebuffer);
    glGenVertexArrays(1, &fullscreenVAO);
}

OutlinePass::~OutlinePass()
{
    glDeleteVertexArrays(1, &fullscreenVAO);
    glDeleteTextures(1, &coverageTexture);
    glDeleteTextures(1, &depthTexture);
    glDeleteRenderbuffers(1, &maskCoverageBuffer);
    glDeleteRenderbuffers(1, &maskDepthBuffer);
    glDeleteFramebuffers(1, &resolveFramebuffer);
    glDeleteFramebuffers(1, &maskFramebuffer);
}


bool OutlinePass::accepts(const Entity& entity) const
{
    return entity.hasAnyFlag(acceptedFlags);
}

void OutlinePass::render(std::span<Entity* const> entities)
{
    if (entities.empty()) {
        return;
    }

    // Render entity surfaces as normal.
    renderSurfaces(entities);

    // Recreate framebuffer attachments if the viewport size has changed.
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int width = viewport[2], height = viewport[3];
    if (width <= 0 || height <= 0) { return; }
    resizeMask(width, height);

    // Store currently bound framebuffer to rebind later.
    GLint targetFramebuffer;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &targetFramebuffer);

    // Render the entity mask (multisampled).
    const float noCoverage[] = { 0.0f, 0.0f, 0.0f, 0.0f };  // RGBA value for samples that don't hit entities.
    const float farDepth = 1.0f;                            // Maximum depth (far plane).
    glBindFramebuffer(GL_FRAMEBUFFER, maskFramebuffer);     // Bind the mask framebuffer.
    glClearBufferfv(GL_COLOR, 0, noCoverage);               // Initially set all sample colors to 0.0
    glClearBufferfv(GL_DEPTH, 0, &farDepth);                // Initially set all sample depths to 1.0

    maskShader->use();
    for (Entity* entity : entities) {
        entity->render(maskShader);
    }

    // Blit maskFramebuffer into resolveFramebuffer, averaging the coverage samples 
    // (depth is not averaged) over each pixel (anti-aliasing).
    glBindFramebuffer(GL_READ_FRAMEBUFFER, maskFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFramebuffer);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // Rebind original target framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, targetFramebuffer);

    // Activate the outline shader and set outline parameters.
    outlineShader->use();
    outlineShader->setInt("coverageMask", 0);
    outlineShader->setInt("depthMask", 1);
    outlineShader->setFloat("thickness", outlineThickness);
    outlineShader->setFloat("depthBias", depthBias);
    outlineShader->setVec3("color", outlineColor);

    // Bind the depth and coverage textures from the resolve framebuffer.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, coverageTexture);

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

void OutlinePass::resizeMask(int width, int height)
{
    if (coverageTexture != 0 && width == maskWidth && height == maskHeight) {
        return;
    }

    // Store currently bound framebuffer to rebind later.
    GLint boundFramebuffer;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundFramebuffer);

    // Delete the old buffers and textures.
    glDeleteRenderbuffers(1, &maskCoverageBuffer);
    glDeleteRenderbuffers(1, &maskDepthBuffer);
    glDeleteTextures(1, &coverageTexture);
    glDeleteTextures(1, &depthTexture);

    // Create the buffer for multisampled mask values for the mask framebuffer (single channel).
    glGenRenderbuffers(1, &maskCoverageBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, maskCoverageBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, MASK_SAMPLES, GL_R8, width, height);

    // Create the buffer for multisampled depth values for the mask framebuffer.
    glGenRenderbuffers(1, &maskDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, maskDepthBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, MASK_SAMPLES, GL_DEPTH_COMPONENT24, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Coverage texture to be written to from the averaged mask coverage buffer in a blit operation.
    // Linearly filtered, so sampling between texels gives a sub-pixel coverage ramp;
    // the default border of zero makes samples beyond the screen edge read as empty.
    glGenTextures(1, &coverageTexture);
    glBindTexture(GL_TEXTURE_2D, coverageTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Depth texture to be written to from the mask depth buffer in a blit operation.
    const float farBorder[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, farBorder);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Attach the mask coverage and depth render buffers to the mask framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, maskFramebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, maskCoverageBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, maskDepthBuffer);
    bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

    // Attach the coverage and depth textures to the resolve framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, resolveFramebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, coverageTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    complete = complete && glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

    // Rebind the original framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, boundFramebuffer);

    if (!complete) {
        throw std::runtime_error("Outline mask framebuffer is incomplete.");
    }

    maskWidth = width;
    maskHeight = height;
}

void OutlinePass::setMaskShader(Shader* maskShader)
{
    this->maskShader = dugl::requireNonNull(maskShader);
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
