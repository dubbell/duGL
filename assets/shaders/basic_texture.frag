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
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;

    sampler2D diffuseMap;
    sampler2D specularMap;
};

uniform Material material;


vec3 computeLight(
    vec3 lightDir, 
    vec3 viewDir, 
    vec3 lightAmbient, 
    vec3 lightDiffuse, 
    vec3 lightSpecular,
    vec3 diffuseColor,
    vec3 specularColor,
    vec3 norm)
{
    vec3 ambient = lightAmbient * diffuseColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightDiffuse * diffuseColor;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * lightSpecular * specularColor;

    return ambient + diffuse + specular;
}


void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec2 flippedTexCoord = vec2(TexCoord.x, 1.0 - TexCoord.y);
    vec3 diffuseColor = material.diffuseColor * texture(material.diffuseMap, flippedTexCoord).rgb;
    vec3 specularColor = material.specularColor * texture(material.specularMap, flippedTexCoord).rgb;
    vec3 norm = normalize(Normal);

    // start with directional light
    vec3 result = computeLight(
        -directionalLight.direction,
        viewDir,
        directionalLight.ambient, 
        directionalLight.diffuse, 
        directionalLight.specular,
        diffuseColor,
        specularColor,
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
            diffuseColor, specularColor, norm) * attenuation;
    }

    FragColor = vec4(result, 1.0);
} 