#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in ivec4 boneSecondIDs;
layout (location = 7) in vec4 boneWeights;
layout (location = 8) in vec4 boneSecondWeights;

const int NUM_CASCADES = 3;
const int MAX_NUM_POINTLIGHTS = 5;
const int MAX_NUM_SPOTLIGHTS = 5;

struct DirectionalLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       

    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
};

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos[MAX_NUM_POINTLIGHTS + MAX_NUM_SPOTLIGHTS];
    vec3 TangentViewPos;
    vec3 TangentFragPos;
	vec4 FragPosSpotLightSpace[MAX_NUM_SPOTLIGHTS];
	vec4 FragPosLightSpace[NUM_CASCADES];
	float ClipSpacePosZ;
} vs_out;

layout(std140, binding = 0) uniform GlobalMatrices
{
    mat4 projection;
    mat4 view;
};

layout(std140, binding = 1) uniform GlobalLights
{
	DirectionalLight dirLight;

	ivec3 numOfLights;

	PointLight pointLights[MAX_NUM_POINTLIGHTS];
	SpotLight spotLights[MAX_NUM_SPOTLIGHTS];
};

layout(std140, binding = 2) uniform GlobalVectors
{
	vec3 viewPos;
};

layout(std140, binding = 3) uniform GlobalShadows
{
	uniform mat4 lightSpaceMatrix[NUM_CASCADES];
	uniform	float cascadeEndClipSpace[NUM_CASCADES];
	uniform mat4 spotLightSpaceMatrix[MAX_NUM_SPOTLIGHTS];
};

uniform mat4 model;

const int MAX_BONES = 100;
uniform mat4 gBones[MAX_BONES];

uniform vec4 clipPlane;

void main()
{
	mat4 boneTransform = gBones[boneIDs[0]] * boneWeights[0];
    boneTransform     += gBones[boneIDs[1]] * boneWeights[1];
    boneTransform     += gBones[boneIDs[2]] * boneWeights[2];
    boneTransform     += gBones[boneIDs[3]] * boneWeights[3];
	boneTransform     += gBones[boneSecondIDs[0]] * boneSecondWeights[0];
    boneTransform     += gBones[boneSecondIDs[1]] * boneSecondWeights[1];
    boneTransform     += gBones[boneSecondIDs[2]] * boneSecondWeights[2];
	boneTransform     += gBones[boneSecondIDs[3]] * boneSecondWeights[3];

	vs_out.FragPos = vec3(model * boneTransform * vec4(position, 1.0));
    vs_out.TexCoords = texCoord;
	for (int i = 0 ; i < NUM_CASCADES * numOfLights.z; i++) {
        vs_out.FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0);
    }
       
	for (int i = 0 ; i < numOfLights.y ; i++) {
		vs_out.FragPosSpotLightSpace[i] = spotLightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0);
    }


    mat3 normalMatrix = transpose(inverse(mat3(model * boneTransform)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = normalize(normalMatrix * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(normalMatrix * bitangent);
    
    mat3 TBN = transpose(mat3(T, B, N));    
	
	for(int i = 0 ; i < numOfLights.x; i++) {
		vs_out.TangentLightPos[i] = TBN * pointLights[i].position;
	}

	for(int i = 0 ; i < numOfLights.y; i++) {
		vs_out.TangentLightPos[i + numOfLights.x] = TBN * spotLights[i].position;
	}

    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos;

    gl_Position = projection * view * model * boneTransform * vec4(position, 1.0);
	vs_out.ClipSpacePosZ = gl_Position.z;
	gl_ClipDistance[0] = dot(model * vec4(position, 1.0), clipPlane);
}