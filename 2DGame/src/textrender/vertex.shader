#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 trans;

void main()
{
    gl_Position = trans*vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}