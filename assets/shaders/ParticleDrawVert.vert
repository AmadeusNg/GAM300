#version 450 core



layout (location = 0) out vec2 vUV;
layout (location = 1) out vec4 vColor;
layout (location = 2) out uint vInterpolate;
layout (location = 3) out uint vAlbedoID;




struct ParticleRender
{
    uvec2 packedScaleX_packedColorY;
};
struct ParticleShared
{
    vec4 position;
};

layout(std430, binding = 35) readonly restrict buffer IndirectCommandIndexed
{
  uint drawIndices[];
};

layout(std430, binding = 36) readonly restrict buffer ParticlesSharedData
{
    ParticleShared particlesShared[];
};

layout(std430, binding = 39) readonly restrict buffer ParticlesRender
{
  ParticleRender particlesRender[];
};

vec2 CreateQuadTexCoord(in uint index)
{
    uint b = 1 << index;
    return vec2((0x3 & b) != 0, (0x9 & b) != 0);
}

layout(push_constant) uniform PushData
{
    mat4 viewProj;
    vec3 cameraRight;
    uint interpolate;
    vec3 cameraUp;
    uint textureID;
};

void main()
{
    vUV = CreateQuadTexCoord(gl_VertexIndex);
    vInterpolate = interpolate;
    vAlbedoID = textureID;

    vec2 pos = vUV - 0.5;

    int index = int(drawIndices[gl_InstanceIndex]);

    ParticleShared particle = particlesShared[index];
    ParticleRender render = particlesRender[index];

    vec2 scale = unpackHalf2x16(render.packedScaleX_packedColorY.x);
    vColor = unpackUnorm4x8(render.packedScaleX_packedColorY.y);

    vec3 vertexPos = particle.position.xyz + cameraRight * pos.x * scale.x + cameraUp * pos.y * scale.y;

    gl_Position = viewProj * vec4(vertexPos, 1.0);
}