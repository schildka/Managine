#version 420 core
out vec4 FragColor;

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

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
	sampler2D texture_normal1;
    float shininess;
}; 

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos[MAX_NUM_POINTLIGHTS + MAX_NUM_SPOTLIGHTS];
    vec3 TangentViewPos;
    vec3 TangentFragPos;
	vec4 FragPosSpotLightSpace[MAX_NUM_SPOTLIGHTS];
	vec4 FragPosLightSpace[NUM_CASCADES];
	float ClipSpacePosZ;
} fs_in;

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

uniform Material material;

//Shadow
uniform sampler2D shadowMap[NUM_CASCADES];	
uniform samplerCube pointDepthMap[MAX_NUM_SPOTLIGHTS];
uniform sampler2D spotDepthMap[MAX_NUM_SPOTLIGHTS];


float farPlane = 25.0f; // ToDo is uniform

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowSpotCalculation(int z, vec4 fragPosSpotLightSpace, SpotLight spotLight, vec3 normal);
float ShadowPointCalculation(int z, vec3 fragPos, PointLight pointLight);
float ShadowDirectionalCalculation(int i, vec4 fragPosLightSpace, vec3 normal);

//function prototypes
vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 color);
vec3 CalcPointLight(int i, PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color);
vec3 CalcSpotLight(int i, SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color);

void main()
{    
     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(material.texture_normal1, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0); // )  // this normal is in tangent space

	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

	// get diffuse color
    vec3 color = texture(material.texture_diffuse1, fs_in.TexCoords).rgb;

	
	vec3 result = numOfLights.z != 0 ? CalcDirLight(dirLight, normal, viewDir, color) : vec3(0.0, 0.0, 0.0);

	for(int i = 0; i < numOfLights.x; i++){
		result += CalcPointLight(i, pointLights[i], normal, fs_in.FragPos, viewDir, color);  
	}
          

	for(int i = 0; i < numOfLights.y; i++){
		result += CalcSpotLight(i, spotLights[i], normal, fs_in.FragPos, viewDir, color); 
	}

    FragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 finalColor)
{
	// ambient
	vec3 ambient = light.ambient;

    // diffuse 
	vec3 lightDir = normalize(-dirLight.direction); // direction
    float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = light.diffuse * diff;
   
	// specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular1, fs_in.TexCoords).rgb; 

	vec3 CascadeIndicator = vec3(0.0, 0.0, 0.0);
	float shadow = 0.0;
	for (int i = 0; i < NUM_CASCADES; i++) {
        if (fs_in.ClipSpacePosZ <= cascadeEndClipSpace[i]) {// Das hier failt!!!! 
		    shadow = ShadowDirectionalCalculation(i, fs_in.FragPosLightSpace[i], normal);   

			// Fehler liegt darin das wir hier nie reinkommen!
			if (i == 0) 
                CascadeIndicator = vec3(0.0, 0.0, 0.5);
            else if (i == 1)
                CascadeIndicator = vec3(0.0, 0.5, 0.0);
            else if (i == 2)
                CascadeIndicator = vec3(0.5, 0.0, 0.0);

            break;
        }
    }
               
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * finalColor;// + CascadeIndicator;    

    return result;
}

// calculates the color when using a point light.
vec3 CalcPointLight(int i, PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 finalColor)
{
    vec3 lightDir = normalize(fs_in.TangentLightPos[i] - fs_in.TangentFragPos);
    // diffuse shading
    float diff = max(dot(lightDir, normal), 0.0);
	// specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
	vec3 specular = light.specular * spec * texture(material.texture_specular1, fs_in.TexCoords).rgb; 
    ambient *= attenuation;
    diffuse *= attenuation;
	specular *= attenuation;

	float shadow = 0.0;

	shadow = ShadowPointCalculation(i, fs_in.FragPos, light); 		


    return (ambient + (1.0 - shadow) * (diffuse + specular)) * finalColor;
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(int i, SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 finalColor)
{
    vec3 lightDir = normalize(fs_in.TangentLightPos[i+numOfLights.x] - fs_in.TangentFragPos);
    // diffuse shading
    float diff = max(dot(lightDir, normal), 0.0);
	// specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
	vec3 specular = light.specular * spec * texture(material.texture_specular1, fs_in.TexCoords).rgb; ;
	
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	float shadow = 0.0;
	shadow = ShadowSpotCalculation(i, fs_in.FragPosSpotLightSpace[i], light, normal);


    return (ambient + (1.0 - shadow) * (diffuse + specular)) * finalColor;
}

float ShadowSpotCalculation(int z, vec4 fragPosSpotLightSpace, SpotLight spotLight, vec3 normal)
{
    // get vector between fragment position and light position
    vec3 projCoords = fragPosSpotLightSpace.xyz / fragPosSpotLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
    // use the fragment to light vector to sample from the depth map    
    float closestDepth = texture(spotDepthMap[z], projCoords.xy).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    // closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = projCoords.z;
    
    vec3 lightDir = normalize(spotLight.position - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);


    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(spotDepthMap[z], 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(spotDepthMap[z], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

float ShadowPointCalculation(int z, vec3 fragPos, PointLight pointLight)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - pointLight.position;
    // use the fragment to light vector to sample from the depth map    
    // float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    // closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    // float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    // float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // PCF
    // float shadow = 0.0;
    // float bias = 0.05; 
    // float samples = 4.0;
    // float offset = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
        // for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        // {
            // for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            // {
                // float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                // closestDepth *= far_plane;   // Undo mapping [0;1]
                // if(currentDepth - bias > closestDepth)
                    // shadow += 1.0;
            // }
        // }
    // }
    // shadow /= (samples * samples * samples);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(fs_in.TangentViewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointDepthMap[z], fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= farPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    //FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

float ShadowDirectionalCalculation(int i, vec4 fragPosLightSpace, vec3 normal)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap[i], projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    
	
	vec3 lightDir = normalize(-dirLight.direction);
    
	
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
	
	
	// check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap[i], 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap[i], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}