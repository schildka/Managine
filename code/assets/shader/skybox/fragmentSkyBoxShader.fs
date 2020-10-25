#version 420 core
out vec4 FragColor;
out vec4 FragMotionVector;

in vec3 TexCoords;
in vec4 ClipPos;
in vec4 PrevClipPos;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
	
   /* Motion vector (0.5 means no motion) */
   vec3 ndc_pos = (ClipPos / ClipPos.w).xyz;
   vec3 prev_ndc_pos = (PrevClipPos / PrevClipPos.w).xyz;
   FragMotionVector = vec4((ndc_pos - prev_ndc_pos).xy + 0.5, 0, 1);
}