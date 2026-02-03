#include "dugl/shader.h"


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
    catch(const std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        std::cout << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(Shader&& other) noexcept : ID(other.ID)
{
    other.ID = 0;
}

Shader::~Shader()
{
    if (ID != 0) glDeleteProgram(ID);
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