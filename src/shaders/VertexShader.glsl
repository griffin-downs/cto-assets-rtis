#version 300 es
precision highp float;

layout (location=0) in vec3 aPos;

uniform mat4 model, view, projection;

out vec3 vViewPos;

void main() {
    vec4 viewPos = view * model * vec4(aPos, 1.0);
    vViewPos = viewPos.xyz;
    gl_Position = projection * viewPos;
}
