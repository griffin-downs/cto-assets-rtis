#version 300 es
precision highp float;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 vViewPos;     // view-space position
out vec3 vViewNormal;  // view-space normal

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);

    // World normal (handles non-uniform scale in model)
    vec3 worldN = normalize(mat3(transpose(inverse(model))) * aNormal);

    // To view space
    vViewPos    = (view * worldPos).xyz;
    vViewNormal = normalize(mat3(view) * worldN); // view is rigid

    gl_Position = projection * vec4(vViewPos, 1.0);
}
