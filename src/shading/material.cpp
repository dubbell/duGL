#include "dugl/shading/material.h"
#include "dugl/utils/glad_include.h"


static dugl::uint createSolidTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    dugl::uint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    unsigned char pixel[4] = { r, g, b, a };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

    // a single texel: no filtering to do and no mipmaps to build
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

dugl::uint defaultTextures::white()
{
    // created on first use, which can only happen once a GL context exists. The texture is
    // shared by every material that lacks a map and lives as long as the context does, so
    // there is nothing to release; it would need revisiting if a second context were ever
    // created in one run.
    static dugl::uint texture = createSolidTexture(255, 255, 255, 255);
    return texture;
}
