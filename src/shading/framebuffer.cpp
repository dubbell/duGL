#include "dugl/shading/framebuffer.h"

#include <glm/gtc/type_ptr.hpp>

#include <stdexcept>


static bool isColorAttachment(GLenum attachmentPoint)
{
    return attachmentPoint >= GL_COLOR_ATTACHMENT0 && attachmentPoint <= GL_COLOR_ATTACHMENT31;
}

Framebuffer::Framebuffer(std::vector<FramebufferAttachment> attachments)
    : attachments(std::move(attachments))
{
    glGenFramebuffers(1, &id);
}

Framebuffer::~Framebuffer()
{
    releaseImages();
    glDeleteFramebuffers(1, &id);
}

void Framebuffer::resize(int width, int height)
{
    if (!images.empty() && width == this->width && height == this->height) {
        return;
    }

    GLint boundFramebuffer;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundFramebuffer);

    releaseImages();
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    bool hasColor = false;
    for (const FramebufferAttachment& attachment : attachments)
    {
        dugl::uint image = 0;
        if (attachment.samples > 0)
        {
            glGenRenderbuffers(1, &image);
            glBindRenderbuffer(GL_RENDERBUFFER, image);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, attachment.samples, attachment.internalFormat, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment.attachmentPoint, GL_RENDERBUFFER, image);
        }
        else
        {
            const TextureParameters& parameters = attachment.textureParameters;
            glGenTextures(1, &image);
            glBindTexture(GL_TEXTURE_2D, image);
            glTexStorage2D(GL_TEXTURE_2D, 1, attachment.internalFormat, width, height);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, parameters.minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, parameters.magFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, parameters.wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, parameters.wrap);
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(parameters.borderColor));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, parameters.compareMode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, parameters.compareFunc);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment.attachmentPoint, GL_TEXTURE_2D, image, 0);
        }

        images.push_back(image);
        hasColor = hasColor || isColorAttachment(attachment.attachmentPoint);
    }

    if (!hasColor) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    glBindFramebuffer(GL_FRAMEBUFFER, boundFramebuffer);

    if (!complete) {
        throw std::runtime_error("Framebuffer is incomplete.");
    }

    this->width = width;
    this->height = height;
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

dugl::uint Framebuffer::getId() const
{
    return id;
}

dugl::uint Framebuffer::getImage(size_t index) const
{
    return images.at(index);
}

int Framebuffer::getWidth() const
{
    return width;
}

int Framebuffer::getHeight() const
{
    return height;
}

void Framebuffer::releaseImages()
{
    for (size_t image_i = 0; image_i < images.size(); image_i++)
    {
        if (attachments[image_i].samples > 0) {
            glDeleteRenderbuffers(1, &images[image_i]);
        }
        else {
            glDeleteTextures(1, &images[image_i]);
        }
    }
    images.clear();
}


FramebufferScope::FramebufferScope(const Framebuffer& framebuffer)
{
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousFramebuffer);
    glGetIntegerv(GL_VIEWPORT, previousViewport);
    framebuffer.bind();
    glViewport(0, 0, framebuffer.getWidth(), framebuffer.getHeight());
}

FramebufferScope::~FramebufferScope()
{
    glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
    glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
}
