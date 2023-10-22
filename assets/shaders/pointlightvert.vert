#version 450

const vec2 OFFSET[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0), 
    vec2(1.0, -1.0), 
    vec2(-1.0, 1.0), 
    vec2(1.0, 1.0));


layout (location = 0) out vec2 fragoffset;

struct PointLight {
    vec4 Position;
    vec4 Color;
};

//for the scene
layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 proj;
    mat4 view;
    //mat4 InvView;
    vec4 ambientlightcolor;
    PointLight pointlights[10];
} PL;

layout(push_constant) uniform Push {
    vec4 Pos;
    vec4 Color;
    float radius;
} push;

void main() {
    fragoffset = OFFSET[gl_VertexIndex];
    vec3 WorldCameraRight = {PL.view[0][0], PL.view[1][0], PL.view[2][0]};
    vec3 WorldCameraUp = {PL.view[0][1], PL.view[1][1], PL.view[2][1]};

    vec3 worldPos = push.Pos.xyz + push.radius * fragoffset.x * WorldCameraRight+ push.radius* fragoffset.y * WorldCameraUp;
    gl_Position = PL.proj * PL.view* vec4(worldPos, 1.0);
}
