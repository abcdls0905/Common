#version 330 core

out float clipDepth;

void main()
{
	if (clipDepth < -0.005)
		discard;
}