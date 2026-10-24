#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 6) in uvec4 aBoneIds;
layout (location = 7) in vec4 aWeights;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

const int MAX_BONES = 32;

uniform mat4 model;
uniform mat4 bones[MAX_BONES];
uniform mat4 lightSpace;

layout (std140) uniform Perspective
{
    mat4 view;
    mat4 projection;
};

void main()
{
    mat4 skin = aWeights.x * bones[aBoneIds.x]
              + aWeights.y * bones[aBoneIds.y]
              + aWeights.z * bones[aBoneIds.z]
              + aWeights.w * bones[aBoneIds.w];
    mat4 skinnedModel = model * skin;

    gl_Position = projection * view * skinnedModel * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    Normal = mat3(transpose(inverse(skinnedModel))) * aNormal;
    FragPos = vec3(skinnedModel * vec4(aPos, 1.0));
    FragPosLightSpace = lightSpace * vec4(FragPos, 1.0);
}
