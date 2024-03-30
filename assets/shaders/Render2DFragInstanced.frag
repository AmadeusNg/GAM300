
#version 450

layout(set = 1, binding = 4) uniform sampler2D texArraySampler[1000];

layout(location = 0) in vec4 Color;
layout(location = 1) in vec3 Position;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) flat in uint inTextureIndex; 
layout(location = 4) in flat uint outID;


layout(location = 0) out vec4 outColor;
layout(location = 1) out uint trueID;


void main() 
{
   trueID = outID;
   vec4 texColor = texture(texArraySampler[inTextureIndex], TexCoord);
   outColor = mix(Color, texColor, Color.w);

}