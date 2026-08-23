#pragma once

#include <glm/glm.hpp>

#include "dugl/common.h"


// A material is a set of constant factors, each optionally modulated by a map. Every factor
// is meaningful on its own, so a material without any maps is a perfectly good solid-colour
// material and needs no special handling.
//
// A map set to 0 means "absent". Rather than branching in the shader, the binding code
// substitutes a 1x1 texture holding the neutral value for that map, so the shader always
// samples and absent maps multiply through as 1.
struct Material
{
    glm::vec3 diffuseColor = glm::vec3(1.0f);
    glm::vec3 specularColor = glm::vec3(0.5f);
    float shininess = 32.0f;

    dugl::uint diffuseMap = 0;
    dugl::uint specularMap = 0;
};


namespace defaultTextures
{
    // 1x1 white; the identity for a map that modulates a factor multiplicatively
    dugl::uint white();
}
