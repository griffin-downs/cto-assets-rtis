#version 300 es
precision highp float;

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;

uniform mat4 model, view, projection;

out vec3 vViewPos;
out vec3 vNormalV;

void main() {
    vec4 viewPos = view * model * vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vec3 normalV = normalize(normalMatrix * aNormal);

    vViewPos = viewPos.xyz;
    vNormalV = normalV;
    gl_Position = projection * viewPos;
}
