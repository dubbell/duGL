#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 6) in uvec4 aBoneIds;
layout (location = 7) in vec4 aWeights;

const int MAX_BONES = 32;

uniform mat4 model;
uniform mat4 bones[MAX_BONES];

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

    gl_Position = projection * view * model * skin * vec4(aPos, 1.0);
}
