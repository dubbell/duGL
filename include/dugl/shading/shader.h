#pragma once

#include <glm/glm.hpp>

#include <string>

#include "lighting.h"


class Shader
{
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);

    // enable move functionality
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    // delete copy functionality
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    ~Shader();

    void use();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, glm::vec3 value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;

    void setPerspective(glm::mat4& view, glm::mat4& projection, glm::vec3& position) const;
    void setDirectionalLight(DirectionalLight& directionalLight) const;
    void setPointLights(std::vector<PointLight>& pointLights) const;
};