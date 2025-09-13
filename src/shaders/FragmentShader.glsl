#version 300 es
precision highp float;

in vec3 vViewPos;
out vec4 FragColor;

uniform vec4 color;
uniform mat4 view;

const vec3  kLightColor       = vec3(1.0);
const vec3  kLightPositionW   = vec3(8.0, 6.0, 15.0);
const float kAmbientIntensity = 0.3;
const float kSpecularStrength = 0.5;
const float kShininess        = 64.0;

void main() {
    // Compute face normal in *view* space
    vec3 N = normalize(cross(dFdx(vViewPos), dFdy(vViewPos)));

    // Light vector in view space
    vec3 LposV = (view * vec4(kLightPositionW, 1.0)).xyz;
    vec3 L = normalize(LposV - vViewPos);
    vec3 V = normalize(-vViewPos);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);

    vec3 ambient = kAmbientIntensity * color.rgb;
    vec3 diffuse = color.rgb * kLightColor * NdotL;
    float spec = (NdotL > 0.0) ? pow(NdotH, kShininess) : 0.0;
    vec3 specular = kSpecularStrength * kLightColor * spec;

    vec3 outColor = ambient + diffuse + specular;
    FragColor = vec4(outColor, color.a);
}
