#pragma once

#include "dugl/utils/glad_include.h"
#include "dugl/common.h"

#include <glm/glm.hpp>

#include <vector>


struct TextureParameters
{
    GLenum minFilter = GL_NEAREST;
    GLenum magFilter = GL_NEAREST;
    GLenum wrap = GL_CLAMP_TO_BORDER;
    glm::vec4 borderColor = glm::vec4(0.0f);
    GLenum compareMode = GL_NONE;
    GLenum compareFunc = GL_LEQUAL;
};

struct FramebufferAttachment
{
    GLenum attachmentPoint;
    GLenum internalFormat;
    int samples = 0;
    TextureParameters textureParameters;
};

class Framebuffer
{
private:
    dugl::uint id = 0;
    std::vector<FramebufferAttachment> attachments;
    std::vector<dugl::uint> images;
    int width = 0;
    int height = 0;

public:
    explicit Framebuffer(std::vector<FramebufferAttachment> attachments);
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    /* Change viewport size. */
    void resize(int width, int height);
    /* Bind this framebuffer. */
    void bind() const;

    /* Get the OpenGL framebuffer handle. */
    dugl::uint getId() const;
    /* Get the image handle for a framebuffer attachment. */
    dugl::uint getImage(size_t index) const;
    /* Get the width of the framebuffer viewport. */
    int getWidth() const;
    /* Get the height of the framebuffer viewport. */
    int getHeight() const;

private:
    void releaseImages();
};

class FramebufferScope
{
private:
    GLint previousFramebuffer = 0;
    GLint previousViewport[4] = {};

public:
    explicit FramebufferScope(const Framebuffer& framebuffer);
    ~FramebufferScope();

    FramebufferScope(const FramebufferScope&) = delete;
    FramebufferScope& operator=(const FramebufferScope&) = delete;
};
