#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 texCoord;

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
	vec4 Color;
	vec3 Normal;
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

layout(std140, binding = 3) uniform GlobalShadows
{
	uniform mat4 lightSpaceMatrix[NUM_CASCADES];
	uniform	float cascadeEndClipSpace[NUM_CASCADES];
	uniform mat4 spotLightSpaceMatrix[MAX_NUM_SPOTLIGHTS];
};

uniform mat4 model;

uniform vec4 clipPlane;

void main()
{
	vs_out.FragPos = vec3(model * vec4(position, 1.0));   
    vs_out.TexCoords = texCoord;
	vs_out.Color = color;
	vs_out.Normal = mat3(transpose(inverse(model))) * normal;

	for (int i = 0 ; i < NUM_CASCADES * numOfLights.z; i++) {
        vs_out.FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0);
    }

	for (int i = 0 ; i < numOfLights.y ; i++) {
		vs_out.FragPosSpotLightSpace[i] = spotLightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0);
    }

    gl_Position = projection * view * model * vec4(position, 1.0);
	vs_out.ClipSpacePosZ = gl_Position.z;
	gl_ClipDistance[0] = dot(model * vec4(position, 1.0), clipPlane);
}