#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;   
    sampler2D shadow;
	sampler2D shadowback;
    float shininess;
}; 

struct Light {
    //vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in vec4 FragPosLightSpace;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

//point light
uniform vec3 light_World_Pos;
uniform mat4 worldview; 
uniform float near;
uniform float far;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(material.shadow, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);  
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(material.shadow, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(material.shadow, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 0.0 : 1.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

float CalcPointLight()
{
    vec3 vPosDP = light_World_Pos - FragPos;
    //vec3 vPosDP = -(worldview * vec4(FragPos, 1)).xyz;
    float fLength = length(vPosDP);
    vPosDP /= fLength;
    float fDPDepth = 0;
    float fSceneDepth = (fLength - near) / (far - near);
    fSceneDepth = (fSceneDepth + 1.0) / 2.0;
    vec2 size = 1 / textureSize(material.shadow, 0);
    if(vPosDP.z >= 0.0)
    {      
        vec2 vTexFront = vec2(0, 0);
        vTexFront.x = (vPosDP.x / (1.0 + vPosDP.z)) * 0.5 + 0.5;
        vTexFront.x = 1 - vTexFront.x;
        vTexFront.y = 1.0 - ((vPosDP.y / (1.0 + vPosDP.z)) * 0.5 + 0.5);
        fDPDepth = texture(material.shadow, vTexFront).r;
    }
    else
    {
        vec2 vTexBack = vec2(0, 0);
        vTexBack.x = (vPosDP.x /  (1.0 - vPosDP.z)) * 0.5 + 0.5; 
        vTexBack.x = 1 - vTexBack.x;
        vTexBack.y = 1.0 - ((vPosDP.y /  (1.0 - vPosDP.z)) * 0.5 + 0.5);
        fDPDepth = texture(material.shadowback, vTexBack).r;
    }
    return fSceneDepth > fDPDepth ? 0.0 : 1.0;
}

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    // vec3 lightDir = normalize(light.position - FragPos);
    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
        
    //float shadow = ShadowCalculation(FragPosLightSpace);  
    float shadow = CalcPointLight(); 
    shadow = 1;
    vec3 result = ambient + shadow * (diffuse + specular);
    FragColor = vec4(result, 1.0);
} 