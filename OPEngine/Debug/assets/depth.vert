#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
	vec4 temp = lightSpaceMatrix * model * vec4(aPos, 1.0);
	//temp.z = temp.y + 1.5;
    gl_Position = temp;
}