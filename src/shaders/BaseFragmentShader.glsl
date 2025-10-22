#version 300 es
precision highp float;

in vec3 vViewPos;
in vec3 vNormalV;
out vec4 FragColor;

uniform vec3 uColor;
uniform mat4 view;

const vec3  kLightColor       = vec3(0.90, 0.95, 1.0);
const vec3  kLightPositionW   = vec3(8.0, 6.0, 10.0);
const float kAmbientIntensity = 0.2;
const float kSpecularStrength = 0.9;
const float kShininess        = 96.0;

const float kFresnelPow = 4.0;
const float uSheenGain  = 1.2;

vec3 fresnelGradientFromBase(vec3 base, float t) {
    vec3 c0 = base * 0.9;

    const vec3 innerTarget = vec3(0.95, 0.80, 0.35);
    const vec3 outerTarget = vec3(1.0, 1.0, 1.0);

    vec3 c1 = mix(base, innerTarget, 0.75);

    vec3 towardsOuter = mix(c1, outerTarget, 0.80);
    vec3 c2 = mix(towardsOuter * 1.25, vec3(1.0), 0.35);

    vec3 mid = mix(c0, c1, smoothstep(0.0, 0.5, t));
    return mix(mid, c2, smoothstep(0.5, 1.0, t));
}

void main() {
    vec3 N = normalize(cross(dFdx(vViewPos), dFdy(vViewPos)));
    N = gl_FrontFacing ? N : -N;

    vec3 LposV = (view * vec4(kLightPositionW, 1.0)).xyz;
    vec3 L = normalize(LposV - vViewPos);
    vec3 V = normalize(-vViewPos);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);

    vec3 ambient  = kAmbientIntensity * uColor;
    vec3 diffuse  = uColor * kLightColor * NdotL;
    float spec    = (NdotL > 0.0) ? pow(NdotH, kShininess) : 0.0;
    vec3 specular = kSpecularStrength * kLightColor * spec;
    vec3 blinn    = ambient + diffuse + specular;

    float fresnel = pow(1.0 - clamp(dot(N, V), 0.0, 1.0), kFresnelPow);
    vec3 sheen = fresnelGradientFromBase(uColor, fresnel) * fresnel * uSheenGain;

    vec3 lit = blinn + sheen;
    FragColor = vec4(lit, 1.0);
}
