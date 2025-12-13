#version 460 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D textureSampler;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec4 objectColor = texture(textureSampler, TexCoord);

    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0) * objectColor;
} 