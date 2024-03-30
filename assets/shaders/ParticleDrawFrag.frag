#version 450 core

layout (location = 0) in vec2 TexCoord;
layout (location = 1) in vec4 Color;
layout (location = 2) flat in uint interpolate;
layout (location = 3) flat in uint albedoID;

layout (location = 0) out vec4 outColor;

layout(set = 1, binding = 4) uniform sampler2D texArray[1000];

void main()
{
    vec4 albedo = texture(texArray[albedoID], TexCoord);
    if (interpolate == 1)
    {
        outColor = mix(Color, albedo, Color.a);
    }
    else
    {
        albedo.a = Color.a;
        outColor = albedo;
    }
}