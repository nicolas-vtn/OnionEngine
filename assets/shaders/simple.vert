#version 330 core

uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uViewProj;

uniform mat4 uModel;

layout (location = 0) in vec3 aPos;

void main() {
	gl_Position = uViewProj * uModel * vec4(aPos, 1.0);
}