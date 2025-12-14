#version 460 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 viewPos;

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

struct Material 
{
    sampler2D diffuse;
    int diffuseFlag;
    sampler2D specular;
    int specularFlag;
    float shininess;
};

uniform Material material;

void main()
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    vec3 diffuse;
    if (material.diffuseFlag == 1) 
    {
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoord));
    } 
    else 
    {
        diffuse = vec3(0.0);
    }

    vec3 specular;
    if (material.specularFlag == 1)
    {
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        specular = spec * light.specular * vec3(texture(material.specular, TexCoord));
    }
    else
    {
        specular = vec3(0.0);
    }

    FragColor = vec4(ambient + diffuse + specular, 1.0);
} 