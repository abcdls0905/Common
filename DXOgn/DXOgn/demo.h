#pragma once


#include<d3d11.h>
#include<d3dx11.h>
#include<DxErr.h>
#include <d3dcompiler.h>
#include<xnamath.h>

class CDemo
{
	struct VertexPos
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

	struct CBMVP
	{
		XMMATRIX mvp;
	};

public:
	CDemo();
	~CDemo();
	void Initialize(HWND hwnd);
	bool CompileShader(char* file, char* entry, char* shader_model, ID3DBlob** buffer);
	void Update();
	void Render();
	void ShutDown();
private:
	ID3D11Device* device;
	ID3D11DeviceContext* device_context;
	IDXGISwapChain* swap_chain;
	ID3D11RenderTargetView* render_target;
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* input_layout;
	ID3D11Buffer* vertex_buffer;
	ID3D11Buffer* index_buffer;
	ID3D11ShaderResourceView* shader_resourceview;
	ID3D11SamplerState* sampler_state;
	ID3D11Buffer* mvp_buffer;

	ID3D11Buffer* viewCB_;
	ID3D11Buffer* projCB_;
	ID3D11Buffer* worldCB_;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
};