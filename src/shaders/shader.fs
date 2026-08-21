// #version 330 core
// 
// in float height;
// 
// out vec4 FragColor;
// 
// uniform float minHeight;
// uniform float maxHeight;
// 
// void main()
// {
//   float t = (height - minHeight) / (maxHeight - minHeight);
//   t = clamp(t, 0.0, 1.0);
//   vec3 low  = vec3(0.05, 0.15, 0.50);
//   vec3 mid  = vec3(0.10, 0.75, 0.60);
//   vec3 high = vec3(0.95, 0.35, 0.10);
// 
//   vec3 color;
// 
//   if (t < 0.5)
//       color = mix(low, mid, t * 2.0);
//   else
//       color = mix(mid, high, (t - 0.5) * 2.0);
// 
//   FragColor = vec4(color, 1.0);
// }

// Quick and Dirty Lighting Shader by AI

#version 330 core

in vec3 ViewPos;
in float height;

out vec4 FragColor;

uniform float minHeight;
uniform float maxHeight;

void main()
{
    // Recover a geometric normal directly from the rendered surface.
    vec3 dx = dFdx(ViewPos);
    vec3 dy = dFdy(ViewPos);
    vec3 normal = normalize(cross(dx, dy));

    // Keep the visible side consistently oriented.
    if (!gl_FrontFacing)
        normal = -normal;

    // Light direction in VIEW SPACE.
    // Positive z points roughly toward the camera depending on your convention.
    vec3 lightDir = normalize(-ViewPos);

    float diffuse = max(dot(normal, lightDir), 0.0);

    // Ambient term keeps unlit faces visible.
    float lighting = 0.55 + 0.45 * diffuse;

    // Normalize height to [0,1].
    float range = maxHeight - minHeight;
    float t = 0.5;

    if (abs(range) > 0.000001)
        t = clamp((height - minHeight) / range, 0.0, 1.0);

    // Height gradient.
    vec3 low  = vec3(0.05, 0.15, 0.50);
    vec3 mid  = vec3(0.10, 0.75, 0.60);
    vec3 high = vec3(0.95, 0.35, 0.10);

    vec3 baseColor;

    if (t < 0.5)
        baseColor = mix(low, mid, 2.0 * t);
    else
        baseColor = mix(mid, high, 2.0 * (t - 0.5));

    vec3 finalColor = baseColor * lighting;

    FragColor = vec4(finalColor, 1.0);
}
