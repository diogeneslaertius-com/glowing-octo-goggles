#version 430
layout (location = 0) in vec3 a_Pos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(void)
{
    gl_Position = projection * view * model * vec4(a_Pos, 1.0);
}