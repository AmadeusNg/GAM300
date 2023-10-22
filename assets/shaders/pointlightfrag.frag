#version 450

layout(location = 0) in vec2 fragoffset;
layout(location = 0) out vec4 OutColor;

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
    float dis = sqrt(dot(fragoffset, fragoffset));
    if (dis >= 1.0){
        discard;
    }
    float cosDis = 0.5* (cos(dis* 3.1415926538) +1.0);
    OutColor = vec4(push.Color.xyz +0.5* cosDis, cosDis);
}