#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uViewProj;

uniform mat4 uModel;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = uViewProj * uModel * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}