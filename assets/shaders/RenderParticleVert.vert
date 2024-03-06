#version 450 core

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

layout (set = 0,binding = 2) buffer Camera{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
}Cam;

layout (std140, binding = 3) buffer TransformMatrix{
    mat4 List[];
}v_TransformMatrix;

//vertex positions
layout (location = 0) in vec2 in_Position;

//output to fragment shader
layout (location = 0) out vec3 out_Color;

void main(){
    int index = gl_InstanceIndex;

    mat4 currentParticlexform = v_TransformMatrix.List[index];

    //transform the vertex position
    vec4 worldPosition = currentParticlexform * vec4(in_Position, 0.0, 1.0);
    gl_Position = Cam.ProjectionMatrix * Cam.ViewMatrix * worldPosition;

    //output the color
    out_Color = v_Particles.List[index].Color;

    //if the particle is not active, don't draw it
    if(!v_Particles.List[index].Active){
        gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
    }
}