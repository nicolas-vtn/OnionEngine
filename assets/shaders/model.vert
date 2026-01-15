#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 uViewProj;
uniform mat4 uModel;

out vec2 vUV;
out vec3 vNormal;
out vec3 vWorldPos;

void main()
{
    vUV = aUV;

    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;

    // Correct normal transform
    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vNormal = normalize(normalMatrix * aNormal);

    gl_Position = uViewProj * worldPos;
}
