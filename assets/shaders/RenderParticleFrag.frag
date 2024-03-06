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

layout (location = 0) in vec3 color;

layout (location = 0) out vec4 fragColor;

void main(){
    fragColor = vec4(color, 1.0);
}