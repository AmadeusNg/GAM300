#version 450

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vBiTangent;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec3 vNormals;
layout(location = 4) in vec2 vUV;
layout(location = 5) in vec4 vColor;
layout(location = 6) in vec4 BoneIDs;
layout(location = 7) in vec4 Weights;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec4 fragColor;
layout(location = 4) out mat3 fragTBN;
layout(location = 8) out flat uint id;
layout(location = 9) out flat uint textureID;
layout(location = 10) out vec4 clipspacepos;


struct PointLight
{
    vec4 Position;
    vec4 Color;
    vec4 pad[2];
};


layout(set = 0, binding = 0) uniform GlobalUBO{
    mat4 proj;
    mat4 view;
    vec4 ambientlightcolor;
    PointLight pointlights[50];
    int activelights;
    vec4 pad[15];    
}PL;


struct InstanceData
{
    vec4 m_InstanceData; //z = entityID, w = textureID
    mat4 modelMatrix;
    
};

layout(std140, set = 1, binding = 25) readonly buffer instanceDataBuffer
{
    InstanceData instances[];
};

const int MAX_BONES = 80;
layout(set = 2, binding = 4) uniform boneView
{
    mat4 mat[MAX_BONES];
} bones;

void main() 
{

   mat4 meshModel = instances[gl_InstanceIndex].modelMatrix;

   mat4 skinTransform = mat4(0.0f);
   for(int i = 0; i < 4; i++)
   {
        uint j = uint(BoneIDs[i]);
        skinTransform += bones.mat[j] * Weights[i] ;
   }

   const mat4 skinMesh = meshModel;
   
   vec4 position_in_world = skinMesh * skinTransform *vec4(vPosition, 1.0);

   vec4 clipspacepos = PL.proj * PL.view * position_in_world;
   gl_Position = clipspacepos;

   fragPos = position_in_world.xyz;
   fragTexCoord = vUV;
   fragColor = vColor;


   vec3 normal = mat3(transpose(inverse(skinMesh))) * vNormals;
   fragNormal = normal;
   vec3 tanget = normalize(mat3(skinMesh) * vTangent);
   vec3 bitanget = normalize(vec3(vec4(cross(normal, tanget), 0.0)));

   fragTBN = mat3(tanget, bitanget, normal);
   id = uint(instances[gl_InstanceIndex].m_InstanceData.z);


    
}