#version 450 core

layout (set = 0,location = 0) in vec2 in_Position;
layout (set = 0,location = 1) in mat4 ViewMatrix;
layout (set = 0,location = 5) in mat4 ProjectionMatrix;

struct Particle {
    //transform params
    vec3 Size;//xyz scale of the particle
    vec3 Rotation;
    vec3 CurrentPosition;// constant transform position of the entity, position of particle is an offset from here based on velocity and acceleration
    
    //movement params
    vec3 Velocity;
    vec3 Acceleration;
    float Age;//how long has the particle been active
    
    //aesthetics params
    vec3 Color;//rgb color of the particle
    bool Active;//is the particle active
};

layout (std140, binding = 1) buffer Particles {
    Particle List[];
}v_Particles;

layout (std140, binding = 2) buffer TransformMatrix{
    mat4 List[];
}v_TransformMatrix;

//vertex positions


//output to fragment shader
layout (location = 0) out vec3 out_Color;

void main(){
    int index = gl_InstanceIndex;

    mat4 currentParticlexform = v_TransformMatrix.List[index];

    //transform the vertex position
    vec4 worldPosition = currentParticlexform * vec4(in_Position, 0.0, 1.0);
    gl_Position = ProjectionMatrix * ViewMatrix * worldPosition;

    //output the color
    out_Color = v_Particles.List[index].Color;

    //if the particle is not active, don't draw it
    if(!v_Particles.List[index].Active){
        gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
    }
}