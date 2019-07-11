
Texture2D diffuse : register(t0);
SamplerState linefiler : register(s0);

struct VS_Input
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD0;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

PS_Input VS_Main(VS_Input input)
{
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = input.pos;
	vsOut.uv = input.uv;
	return vsOut;
}

float4 PS_Main(PS_Input input) : SV_TARGET
{
	float4 color = diffuse.Sample(linefiler, input.uv);
	return color;
}