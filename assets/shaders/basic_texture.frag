#version 460 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2DShadow shadowMap;

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


float computeShadow(vec4 fragPosLightSpace, vec3 norm, vec3 lightDir)
{
    // transform to light space and map [-1,1] -> [0,1]
    vec3 coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    coords = coords * 0.5 + 0.5;
    if (coords.z > 1.0) {
        return 1.0;
    }
    // bias to avoid a surface shading itself
    float bias = max(0.002 * (1.0 - dot(norm, lightDir)), 0.0005);

    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    float lit = 0.0;
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            lit += texture(shadowMap, vec3(coords.xy + vec2(x, y) * texelSize, coords.z - bias));
        }
    }
    return lit / 9.0;
}

vec3 computeLight(
    vec3 lightDir,
    vec3 viewDir,
    vec3 lightAmbient,
    vec3 lightDiffuse,
    vec3 lightSpecular,
    vec3 diffuseColor,
    vec3 specularColor,
    vec3 norm,
    float shadow)
{
    vec3 ambient = lightAmbient * diffuseColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightDiffuse * diffuseColor;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * lightSpecular * specularColor;

    return ambient + shadow * (diffuse + specular);
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
        norm,
        computeShadow(FragPosLightSpace, norm, -directionalLight.direction));

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
            diffuseColor, specularColor, norm, 1.0) * attenuation;
    }

    FragColor = vec4(result, 1.0);
} 