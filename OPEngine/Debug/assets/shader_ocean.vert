#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;   
    sampler2D shadow;
	sampler2D shadowback;
    float shininess;
}; 

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

uniform Material material;

const int width = 1;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    
    vec2 uv_offset = TexCoords / width;
    vec3 offset = texture(material.specular, uv_offset).rgb;

    const int max_value = 6;
    offset *= (2 * max_value);
    offset -= max_value;
    FragPos += offset;
    vec4 out_pos = projection * view * vec4(FragPos, 1.0);

    gl_Position = out_pos;
}