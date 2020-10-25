#version 420 core
layout (location = 0) in vec3 position;

out vec3 TexCoords;
out vec4 ClipPos;
out vec4 PrevClipPos;

layout(std140, binding = 0) uniform GlobalMatrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat4 prevMVP;

void main()
{
    TexCoords = position;
    vec4 pos = projection * view * model * vec4(position, 1.0);
    gl_Position = pos.xyww;
	
   ClipPos = gl_Position;
   PrevClipPos = prevMVP * vec4(position, 1.0);
}  