
extern matrix model_view_matrix;

extern matrix model_view_proj_matrix;

extern vector color;
extern float4 light_dir;

struct VS_INPUT
{
	float4 position : POSITION;
	float4 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 position : POSITION;
	float4 uv : TEXCOORD;
	float4 color : DIFFUSE;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT o = (VS_OUTPUT)0;
	return o;
}