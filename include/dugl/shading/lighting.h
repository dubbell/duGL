#pragma once

#include <glm/glm.hpp>

#include <vector>

struct DirectionalLight
{
    float yaw;
    float pitch;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

inline glm::vec3 directionalLightDirection(const DirectionalLight& light)
{
    return glm::normalize(glm::vec3(
        glm::cos(glm::radians(light.yaw)) * glm::cos(glm::radians(light.pitch)),
        glm::sin(glm::radians(light.pitch)),
        glm::sin(glm::radians(light.yaw)) * glm::cos(glm::radians(light.pitch))
    ));
}

struct PointLight
{
    glm::vec3 position;
    
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};