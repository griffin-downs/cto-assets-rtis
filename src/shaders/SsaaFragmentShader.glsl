#version 300 es
precision highp float;

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uScene;
uniform vec2 uInverseResolution;

const bool doGamma = false;
const float scale = 4.0;
const float tentMix = 0.75;

void main() {
    float lod = log2(scale);
    vec3 mipAvg = textureLod(uScene, vUV, lod).rgb;

    vec2 off = 0.5 * uInverseResolution;
    vec3 c00 = texture(uScene, vUV + vec2(-off.x, -off.y)).rgb;
    vec3 c10 = texture(uScene, vUV + vec2( off.x, -off.y)).rgb;
    vec3 c01 = texture(uScene, vUV + vec2(-off.x,  off.y)).rgb;
    vec3 c11 = texture(uScene, vUV + vec2( off.x,  off.y)).rgb;
    vec3 tent = 0.375 * (c00 + c11) + 0.125 * (c10 + c01);

    vec3 linear = mix(mipAvg, tent, tentMix);

    vec3 outRgb = doGamma ? pow(linear, vec3(1.0/2.2)) : linear;
    FragColor = vec4(outRgb, 1.0);
}
