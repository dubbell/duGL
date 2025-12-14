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
    sampler2D diffuse;
    int diffuseFlag;
    sampler2D specular;
    int specularFlag;
    float shininess;
};

uniform Material material;


vec3 computeLight(vec3 lDir, vec3 vDir, vec3 lAmb, vec3 lDiff, vec3 lSpec, vec3 dText, vec3 sText, vec3 norm)
{
    vec3 ambient = lAmb * vec3(texture(material.diffuse, TexCoord));

    vec3 diffuse;
    if (material.diffuseFlag == 1) 
    {
        float diff = max(dot(norm, lDir), 0.0);
        diffuse = diff * lDiff * dText;
    } 
    else 
    {
        diffuse = vec3(0.0);
    }

    vec3 specular;
    if (material.specularFlag == 1)
    {
        vec3 reflectDir = reflect(-lDir, norm);
        float spec = pow(max(dot(vDir, reflectDir), 0.0), material.shininess);
        specular = spec * lSpec * sText;
    }
    else
    {
        specular = vec3(0.0);
    }

    return ambient + diffuse + specular;
}


void main()
{
    vec3 vDir = normalize(viewPos - FragPos);
    vec3 dText = vec3(texture(material.diffuse, TexCoord));
    vec3 sText = vec3(texture(material.specular, TexCoord));
    vec3 norm = normalize(Normal);

    // start with directional light
    vec3 result = computeLight(
       -directionalLight.direction,
        vDir,
        directionalLight.ambient, 
        directionalLight.diffuse, 
        directionalLight.specular,
        dText, sText, norm);

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
            vDir, 
            pointLight.ambient, 
            pointLight.diffuse, 
            pointLight.specular,
            dText, sText, norm) * attenuation;
    }

    FragColor = vec4(result, 1.0);
} 