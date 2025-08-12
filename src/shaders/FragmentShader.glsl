#version 300 es
precision mediump float;

in vec3 vNormal;
out vec4 FragColor;

void main()
{
    // Normalize and remap from [-1,1] → [0,1]
    vec3 color = normalize(vNormal) * 0.5 + 0.5;
    FragColor = vec4(color, 1.0);
}
