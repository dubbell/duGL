#pragma once

#include <glm/glm.hpp>

#include <string>
#include <unordered_set>

#include "lighting.h"
#include "material.h"
#include "dugl/modelling/vertex_layout.h"


// Interface for an OpenGL shader program in the GPU.
class Shader
{
private:
    /* Vertex attribute locations in the linked shader program. */
    VertexAttributeMask requiredAttributes;
    /* Active uniforms in the linked shader program. */
    std::unordered_set<std::string> activeUniforms;
    
public:
    dugl::uint ID;

    Shader(const char* vertexPath, const char* fragmentPath);

    // enable move functionality
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    // delete copy functionality
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    ~Shader();

    void use();

    VertexAttributeMask getRequiredAttributes() const { return requiredAttributes; }

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, glm::vec3 value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;

    // sets the material's factors and binds its maps, substituting neutral defaults for
    // maps the material doesn't have
    void setMaterial(const Material& material) const;

    void setPerspective(glm::mat4& view, glm::mat4& projection, glm::vec3& position) const;
    void setDirectionalLight(DirectionalLight& directionalLight) const;
    void setPointLights(std::vector<PointLight>& pointLights) const;

    bool hasActiveUniform(const char* uniformName);

private:
    /* Query the shader program for the active attributes */
    void queryRequiredAttributes();
    /* Query the shader program for the active uniforms. */
    void queryActiveUniforms();
};