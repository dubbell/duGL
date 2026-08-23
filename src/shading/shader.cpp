#include "dugl/shading/shader.h"
#include "dugl/utils/glad_include.h"

#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <fstream>
#include <format>
#include <stdexcept>
#include <algorithm>


Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (const std::ifstream::failure& e)
    {
        throw std::runtime_error(std::format(
            "Failed to read shader file(s) '{}', '{}': {}", vertexPath, fragmentPath, e.what()));
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    int success;
    char infoLog[512];

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, sizeof(infoLog), NULL, infoLog);
        glDeleteShader(vertex);
        throw std::runtime_error(std::format("Vertex shader '{}' failed to compile:\n{}", vertexPath, infoLog));
    }

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, sizeof(infoLog), NULL, infoLog);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        throw std::runtime_error(std::format("Fragment shader '{}' failed to compile:\n{}", fragmentPath, infoLog));
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    // shaders are flagged for deletion but not actually freed until detached from the program,
    // so this is safe to do unconditionally, whether or not linking succeeded
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (!success)
    {
        glGetProgramInfoLog(ID, sizeof(infoLog), NULL, infoLog);
        glDeleteProgram(ID);
        throw std::runtime_error(std::format(
            "Shader program ('{}', '{}') failed to link:\n{}", vertexPath, fragmentPath, infoLog));
    }

    queryRequiredAttributes();
}

// a matrix attribute occupies one location per column, an array one per element
static dugl::uint attributeLocationSpan(unsigned int type, int size)
{
    dugl::uint columns = 1;
    switch (type)
    {
        case GL_FLOAT_MAT2: columns = 2; break;
        case GL_FLOAT_MAT3: columns = 3; break;
        case GL_FLOAT_MAT4: columns = 4; break;
        default: break;
    }

    return columns * (dugl::uint)std::max(size, 1);
}

void Shader::queryRequiredAttributes()
{
    requiredAttributes = 0;

    int attributeCount = 0, maxNameLength = 0;
    glGetProgramiv(ID, GL_ACTIVE_ATTRIBUTES, &attributeCount);
    glGetProgramiv(ID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);

    std::string name(std::max(maxNameLength, 1), '\0');

    for (int i = 0; i < attributeCount; i++)
    {
        int size = 0;
        unsigned int type = 0;
        glGetActiveAttrib(ID, i, (int)name.size(), nullptr, &size, &type, name.data());

        int location = glGetAttribLocation(ID, name.c_str());

        // built-ins such as gl_VertexID are reported as active but aren't fed from a buffer
        if (location < 0) continue;

        for (dugl::uint span = attributeLocationSpan(type, size), j = 0; j < span; j++)
        {
            requiredAttributes |= VertexAttributeMask{1} << (location + j);
        }
    }
}

Shader::Shader(Shader&& other) noexcept : requiredAttributes(other.requiredAttributes), ID(other.ID)
{
    other.ID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        if (ID != 0) {
            glDeleteProgram(ID);
        }

        ID = other.ID;
        requiredAttributes = other.requiredAttributes;
        other.ID = 0;
    }
    return *this;
}

Shader::~Shader()
{
    if (ID != 0) {
        glDeleteProgram(ID);
    }
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, glm::vec3 value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

// texture units the material's maps are bound to
static const int DIFFUSE_MAP_UNIT = 0;
static const int SPECULAR_MAP_UNIT = 1;

void Shader::setMaterial(const Material& material) const
{
    setVec3("material.diffuseColor", material.diffuseColor);
    setVec3("material.specularColor", material.specularColor);
    setFloat("material.shininess", material.shininess);

    glActiveTexture(GL_TEXTURE0 + DIFFUSE_MAP_UNIT);
    glBindTexture(GL_TEXTURE_2D, material.diffuseMap != 0 ? material.diffuseMap : defaultTextures::white());
    setInt("material.diffuseMap", DIFFUSE_MAP_UNIT);

    glActiveTexture(GL_TEXTURE0 + SPECULAR_MAP_UNIT);
    glBindTexture(GL_TEXTURE_2D, material.specularMap != 0 ? material.specularMap : defaultTextures::white());
    setInt("material.specularMap", SPECULAR_MAP_UNIT);

    glActiveTexture(GL_TEXTURE0);
}

void Shader::setPerspective(glm::mat4& view, glm::mat4& projection, glm::vec3& position) const
{
    setMat4("view", view);
    setMat4("projection", projection);
    setVec3("viewPos", position);
}

void Shader::setDirectionalLight(DirectionalLight& directionalLight) const
{
    glm::vec3 direction = glm::normalize(glm::vec3(
        cos(glm::radians(directionalLight.yaw)) * cos(glm::radians(directionalLight.pitch)),
        sin(glm::radians(directionalLight.pitch)),
        sin(glm::radians(directionalLight.yaw)) * cos(glm::radians(directionalLight.pitch))
    ));

    setVec3("directionalLight.direction", direction);
    setVec3("directionalLight.ambient", directionalLight.ambient);
    setVec3("directionalLight.diffuse", directionalLight.diffuse);
    setVec3("directionalLight.specular", directionalLight.specular);
}

size_t MAX_POINT_LIGHTS = 8;  // max number of point lights for now

void Shader::setPointLights(std::vector<PointLight>& pointLights) const
{
    setInt("pointLightCount", std::min(pointLights.size(), MAX_POINT_LIGHTS));

    for (size_t p_i = 0; p_i < std::min(pointLights.size(), MAX_POINT_LIGHTS); p_i++)
    {
        const auto& pointLight = pointLights[p_i];

        setVec3(std::format("pointLights[{}].position", p_i), pointLight.position);

        setVec3(std::format("pointLights[{}].ambient", p_i), pointLight.ambient);
        setVec3(std::format("pointLights[{}].diffuse", p_i), pointLight.diffuse);
        setVec3(std::format("pointLights[{}].specular", p_i), pointLight.specular);
        
        setFloat(std::format("pointLights[{}].constant", p_i), pointLight.constant);
        setFloat(std::format("pointLights[{}].linear", p_i), pointLight.linear);
        setFloat(std::format("pointLights[{}].quadratic", p_i), pointLight.quadratic);
    }
}