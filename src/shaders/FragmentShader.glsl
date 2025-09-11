#version 300 es
precision highp float;

in vec3 vViewPos;     // from VS
in vec3 vViewNormal;  // from VS

out vec4 FragColor;

uniform vec4 color;   // rgba from C++
uniform mat4 view;    // already set in your C++ (same as VS)

void main()
{
    // --- Hardcoded lighting & style ---
    const vec3  kLightColor        = vec3(1.0);
    const vec3  kLightPositionW    = vec3(8.0, 6.0, 15.0);
    const float kAmbientIntensity  = 0.4;
    const float kSpecularStrength  = 0.50;
    const float kShininess         = 85.0;

    const float kFacetStrength      = 1.0; // 0=smooth, 1=flat
    const float kNormalTintStrength = 0.75;
    const float kNormalTintContrast = 4.0;
    const float kRimStrength        = 0.15;
    const float kRimExponent        = 2.0;

    // Smooth vs geometric (facet) normal in *view* space
    vec3 Nsmooth = normalize(vViewNormal);
    vec3 Ngeo    = normalize(cross(dFdx(vViewPos), dFdy(vViewPos)));
    if (dot(Ngeo, Nsmooth) < 0.0) Ngeo = -Ngeo; // keep hemisphere
    vec3 N = normalize(mix(Nsmooth, Ngeo, kFacetStrength));

    // Light and view vectors in view space
    vec3 LposV = (view * vec4(kLightPositionW, 1.0)).xyz;
    vec3 L = normalize(LposV - vViewPos);
    vec3 V = normalize(-vViewPos);
    vec3 H = normalize(L + V);

    // Grayscale normal tint (brightness only, no hue shift)
    vec3  tintRGB  = N * 0.5 + 0.5; // [-1,1] → [0,1]
    float tintGray = dot(tintRGB, vec3(0.2126, 0.7152, 0.0722));
    float t = clamp((tintGray - 0.5) * kNormalTintContrast + 0.5, 0.0, 1.0);
    vec3  albedo = color.rgb * mix(1.0, t, kNormalTintStrength);

    // Blinn–Phong
    float NdotL   = max(dot(N, L), 0.0);
    float NdotH   = max(dot(N, H), 0.0);
    vec3  ambient = kAmbientIntensity * albedo;
    vec3  diffuse = albedo * kLightColor * NdotL;
    float spec    = (NdotL > 0.0) ? pow(NdotH, kShininess) : 0.0;
    vec3  specular= kSpecularStrength * kLightColor * spec;

    // Rim light
    float rim    = pow(max(1.0 - max(dot(N, V), 0.0), 0.0), kRimExponent);
    vec3  rimTerm= albedo * (kRimStrength * rim);

    vec3 outColor = ambient + diffuse + specular + rimTerm;
    FragColor = vec4(outColor, color.a);
}
