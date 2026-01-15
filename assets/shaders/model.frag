#version 330 core

in vec2 vUV;
in vec3 vNormal;
in vec3 vWorldPos;

out vec4 FragColor;

uniform sampler2D uAlbedo;
uniform sampler2D uRoughness;

// Lighting
uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform vec3 uAmbient;

// Camera
uniform vec3 uCameraPos;

// Specular strength (still useful)
uniform float uSpecularStrength;

void main()
{
    vec3 albedo = texture(uAlbedo, vUV).rgb;

    // --- Normalized vectors ---
    vec3 N = normalize(vNormal);
    vec3 L = normalize(-uLightDir);
    vec3 V = normalize(uCameraPos - vWorldPos);

    // --- Diffuse ---
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = albedo * uLightColor * NdotL;

    // --- Roughness -> Shininess ---
    float roughness = texture(uRoughness, vUV).r;

    // Map roughness [0..1] -> shininess [128..4]
    float shininess = mix(128.0, 4.0, roughness);

    // --- Specular (Blinn–Phong) ---
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    float spec = pow(NdotH, shininess);

    vec3 specular = uLightColor * spec * uSpecularStrength;

    // --- Ambient ---
    vec3 ambient = albedo * uAmbient;

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}
