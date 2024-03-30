
#version 450

//layout(binding = 4) uniform sampler2D texSampler;

layout(set = 1, binding = 4) uniform sampler2D texArraySampler[1000];

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec4 fragColor;
layout(location = 4) in mat3 fragTBN;
layout(location = 8) in flat uint id;
layout(location = 9) in flat uint textureID;
layout(location = 10) in vec4 clipspacepos;

layout(location = 0) out vec4 outColor;
layout(location = 1) out uint outID;

struct PointLight
{
    vec4 Position;
    vec4 Color;
    vec4 pad[2];
};

layout(set = 0, binding = 0) uniform GlobalUBO{
    mat4 proj;
    mat4 view;
    //mat4 InvView;
    vec4 ambientlightcolor;
    PointLight pointlights[50];
    int activelights;
    vec4 pad[15];    
}PL;


float constant=0.1;
float linear=0.01;
float quadratic=0.005;
void main()
 {
    if (abs(clipspacepos.x)>1.0||abs(clipspacepos.y)>1.0 || abs(clipspacepos.z)>1.0) discard;
    
    int textureID = int(textureID);
    vec3 ambientlight = PL.ambientlightcolor.xyz * PL.ambientlightcolor.w;
    vec3 SurfaceNormal = normalize(fragNormal);
    vec3 diffuselight = vec3(0.0,0.0,0.0);


    for (int i=0; i<PL.activelights; ++i)
    {
        PointLight currentlight = PL.pointlights[i];
        vec3 directiontolight = PL.pointlights[i].Position.xyz - fragPos;
        
        float atten = 1.0/(constant + (length(directiontolight)* linear) + (length(directiontolight)*quadratic));
        directiontolight = normalize(directiontolight);
        
        float cosAngleIncidence = max(dot(SurfaceNormal, directiontolight),0.0);
        vec3 intensity = currentlight.Color.xyz * currentlight.Color.w * 5 * atten;
        
        diffuselight += intensity * cosAngleIncidence;
    }
    outID = id;
    vec4 texColor = texture(texArraySampler[textureID], fragTexCoord);
    outColor = texColor * vec4((diffuselight + ambientlight), 1.0);
}