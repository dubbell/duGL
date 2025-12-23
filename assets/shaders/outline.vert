#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform float thickness;

uniform mat4 model;

layout (std140) uniform Perspective
{
    mat4 view;
    mat4 projection;
};

void main()
{
    gl_Position = projection * view * model * vec4(aPos + thickness * aNormal, 1.0);
}