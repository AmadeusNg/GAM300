#version 450

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in vec4 BGColor;
layout(location = 4) in vec4 FGColor;
layout(location = 5) in vec2 TextureCoord;
layout(location = 6) in float InPxRange;
layout(location = 7) flat in uint TextureIndex; 

layout(set = 1, binding = 4) uniform sampler2D msdfArray[1000];

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

vec4 linearToGamma(vec4 linearColor)
 {
    bvec4 condition = lessThanEqual(linearColor, vec4(0.0031308));
    vec4 linearPortion = linearColor * 12.92;
    vec4 gammaPortion = pow(linearColor, vec4(1.0/2.4)) * 1.055 - 0.055;
    return mix(gammaPortion, linearPortion, vec4(condition));
}

float screenPxRange() 
{
    vec2 unitRange = vec2(InPxRange)/vec2(textureSize(msdfArray[TextureIndex], 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(TexCoord);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}
layout(location = 0) out vec4 outColor;
void main() 
{
    vec3 msd = texture(msdfArray[TextureIndex], TexCoord).rgb;


    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange()*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

    if (opacity < 0.3) 
    {
        discard;
    }
    vec4 gammaBgColor = linearToGamma(BGColor);
    vec4 gammaFgColor = linearToGamma(FGColor);

    vec4 color = mix(gammaBgColor, gammaFgColor, opacity);
    outColor = color;
}
