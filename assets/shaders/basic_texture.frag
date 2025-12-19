#version 460 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 viewPos;

struct DirectionalLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight directionalLight;

struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

#define MAX_LIGHTS 8
uniform PointLight pointLights[MAX_LIGHTS];
uniform int pointLightCount;


struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
};

uniform Material material;


vec3 computeLight(
    vec3 lightDir, 
    vec3 viewDir, 
    vec3 lightAmbient, 
    vec3 lightDiffuse, 
    vec3 lightSpecular, 
    vec3 diffuseTexture, 
    vec3 specularTexture, 
    vec3 norm)
{
    vec3 ambient = lightAmbient * diffuseTexture;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightDiffuse * diffuseTexture;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = spec * lightSpecular * specularTexture;
    
    return ambient + diffuse + specular;
}


void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec2 flippedTexCoord = vec2(TexCoord.x, 1.0 - TexCoord.y);
    vec3 diffuseTexture = (
        vec3(texture(material.texture_diffuse1, flippedTexCoord)) + 
        vec3(texture(material.texture_diffuse2, flippedTexCoord)) + 
        vec3(texture(material.texture_diffuse3, flippedTexCoord))) / 3.0;
    vec3 specularTexture = (
        vec3(texture(material.texture_specular1, flippedTexCoord)) +
        vec3(texture(material.texture_specular2, flippedTexCoord))) / 2.0;
    vec3 norm = normalize(Normal);

    // start with directional light
    vec3 result = computeLight(
        -directionalLight.direction,
        viewDir,
        directionalLight.ambient, 
        directionalLight.diffuse, 
        directionalLight.specular,
        diffuseTexture, 
        specularTexture, 
        norm);

    for (int i = 0; i < min(pointLightCount, MAX_LIGHTS); i++)
    {
        PointLight pointLight = pointLights[i];
        vec3 pointLightDir = normalize(pointLight.position - FragPos);
        float pointLightDistance = length(pointLight.position - FragPos);
        float attenuation = 1.0 / (pointLight.constant + 
                                pointLight.linear * pointLightDistance + 
                                pointLight.quadratic * (pointLightDistance * pointLightDistance));
        
        // add point light
        result += computeLight(
            pointLightDir, 
            viewDir, 
            pointLight.ambient, 
            pointLight.diffuse, 
            pointLight.specular,
            diffuseTexture, specularTexture, norm) * attenuation;
    }

    FragColor = vec4(result, 1.0);
} 