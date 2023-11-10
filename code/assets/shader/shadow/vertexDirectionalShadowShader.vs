#version 420 core
layout (location = 0) in vec3 position;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in ivec4 boneSecondIDs;
layout (location = 7) in vec4 boneWeights;
layout (location = 8) in vec4 boneSecondWeights;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

const int MAX_BONES = 100;
uniform mat4 gBones[MAX_BONES];

uniform bool isSkeletalMesh;

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

	if(isSkeletalMesh){
		gl_Position = lightSpaceMatrix * model * boneTransform * vec4(position, 1.0);
	}else{
		gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);
	}
    
}