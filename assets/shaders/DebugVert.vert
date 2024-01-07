#version 450
layout(location = 0) in vec3 vPosition;

layout(location = 0) out vec3 fragColor;

struct DebugPushConstants {
	mat4 TransformMatrix;
	vec4 DebugColor;
};

layout(std140, binding = 11) readonly buffer ShaderDataBuffer
{
    DebugPushConstants Instance[];
};

layout (push_constant) uniform Push{
	mat4 Proj;
	mat4 View;
}push;


void main(){
    mat4 xform = Instance[gl_InstanceIndex].TransformMatrix;
    fragColor = Instance[gl_InstanceIndex].DebugColor.xyz;
    gl_Position = push.Proj * push.View * xform * vec4(vPosition,1.0);
}