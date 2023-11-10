#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out DATA {
	vec4 worldPosition;
	vec2 textureCoord;
	vec3 normal;
	vec3 toCamera;
	vec3 fromLight;
} Out;

layout(std140, binding = 0) uniform GlobalMatrices
{
    mat4 projection;
    mat4 view;
};

layout(std140, binding = 2) uniform GlobalVectors
{
	vec3 viewPos;
};

uniform mat4 model;

uniform float textureTiling;
uniform vec3 lightDirection;

void main()
{
	vec4 worldPosition = model * vec4(position,1);
	vec4 positionRelativeToCamera = view * worldPosition;	// ToDo view might be cameraPos but i don't think so...

	Out.worldPosition = projection * positionRelativeToCamera;
	Out.textureCoord = texCoord * textureTiling;
	Out.normal = normal;
	Out.toCamera = viewPos - worldPosition.xyz;
	Out.fromLight = lightDirection;

	gl_Position = Out.worldPosition;
}  