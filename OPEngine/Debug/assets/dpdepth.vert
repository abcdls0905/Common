
layout (location = 0) in vec3 aPos;

uniform mat4 worldview;  
uniform mat4 model;
uniform vec3 light_World_Pos;

uniform float near;
uniform float far;
uniform float dir;

out float clipDepth;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0);
	//vec4 pos = worldview * worldPos;
	vec4 pos = light_World_Pos - worldPos;
	pos /= pos.w;

	pos.z *= dir;
		
	float fLength = length(pos.xyz);

	pos /= fLength;
	clipDepth = pos.z;

	pos.x /= (pos.z + 1.0);
	pos.y /= (pos.z + 1.0);

	pos.z = (fLength - near) / (far - near);
	pos.w = 1.0;
	
    gl_Position = pos;
}
